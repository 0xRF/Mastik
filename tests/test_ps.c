#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>

#include <mastik/low.h>
#include <mastik/prime.h>
#include <mastik/list_traverse.h>
#include <mastik/list_utils.h>
#include <mastik/ps_evset.h>

#define VICTIM_READ_ACCESS(x)   ({                                \
  *synchronization_params = (volatile uint64_t)x;                  \
  *synchronization = 11;                                           \
  while(*synchronization==11);                                     })


int main() {


  uint64_t target_addr = 0;


  int thrL1 = 43;
  int thrLLC = 111;
  int thrRAM = 250;
  int thrDET = 203;

  printf("\nThresholds Configured\n\n");
  printf("\tL1/L2    : %u\n", thrL1   );
  printf("\tLLC      : %u\n", thrLLC  );
  printf("\tRAM      : %u\n", thrRAM  );
  printf("\tTHRESHOLD: %u\n", thrDET  );


  //////////////////////////////////////////////////////////////////////////////
  // Eviction Set Construction

  #define EV_LLC LLC_WAYS

#if PREMAP_PAGES == 1
  ps_evset_premap(evict_mem);
#endif

  Elem  *evsetList;
  Elem **evsetList_ptr = &evsetList;

  *evsetList_ptr=NULL;

repeat_evset:
  if (PS_SUCCESS != ps_evset( evsetList_ptr,
                              (char*)target_addr,
                              EV_LLC,
                              evict_mem,
                              HUGE_PAGES_AVAILABLE,
                              thrDET))
    goto repeat_evset;

  printf("\nEviction set is constructed successfully\n\n");

  printf("\tEviction set addresses are: "); print_list(evsetList);

  //////////////////////////////////////////////////////////////////////////////
  // Prepare for Test

  // Convert the eviction set link-list to an array
  uint64_t evset[EV_LLC]; list_to_array(evsetList, evset);
  
  // Set its first element as the scope_addr
  uint64_t scope_addr = evset[0];

  //////////////////////////////////////////////////////////////////////////////
  // Prime+Scope (Toy Example)

  printf("\nTesting Prime+Scope\n\n");

  int access_time, success=0;

#define TIME_READ_ACCESS(x) access_time = no_lfence_memaccesstime((void*)x)

  for (int t=0; t<TEST_LEN; t++) {

    PRIME(evset);
    TIME_READ_ACCESS(scope_addr);
    TIME_READ_ACCESS(scope_addr);

    // Sanity check: the scope_addr is in low-level cache (L1).

    if (access_time<thrLLC)  
          printf("\tSuccess at test %d\n", t);
    else  printf("\tFailure at test %d\n", t);

    ////////////////////////////////////////////////////////////////////////////

    PRIME(evset);
    TIME_READ_ACCESS(scope_addr);    // SCOPE 
    TIME_READ_ACCESS(scope_addr);    // SCOPE
    VICTIM_READ_ACCESS(target_addr); // Cross-core access to monitored set
    TIME_READ_ACCESS(scope_addr);    // SCOPE detects access

    // The victim access to the target address should evict the scope_addr from all cache levels.

    if (access_time>thrLLC) { 
          success++;
          printf("\tSuccess at test %d\n\n", t);}
    else  printf("\tFailure at test %d\n\n", t);
  }

  printf("\tSuccess %d/%d\n", success, TEST_LEN);
}
