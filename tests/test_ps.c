#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <mastik/list_traverse.h>
#include <mastik/list.h>
#include <mastik/low.h>
#include <mastik/prime.h>
#include <mastik/ps.h>

// #define VICTIM_READ_ACCESS(x)   ({                                \
//   *synchronization_params = (volatile uint64_t)x;                  \
//   *synchronization = 11;                                           \
//   while(*synchronization==11);                                     })

int main() {

  int thrL1 = 43;
  int thrLLC = 111;
  int thrRAM = 250;
  int thrDET = 203;

  printf("\nThresholds Configured\n\n");
  printf("\tL1/L2    : %u\n", thrL1);
  printf("\tLLC      : %u\n", thrLLC);
  printf("\tRAM      : %u\n", thrRAM);
  printf("\tTHRESHOLD: %u\n", thrDET);

  uint64_t *evict_mem = (uint64_t *)malloc(EVICT_LLC_SIZE);

  uint64_t target_addr = (uintptr_t)evict_mem + 8 * (rand() % 32);
  //////////////////////////////////////////////////////////////////////////////
  // Eviction Set Construction

#define EV_LLC LLC_WAYS

  ps_evset_premap(evict_mem);

  node *evsetList;
  list evsetList_ptr = &evsetList;

  *evsetList_ptr = NULL;

repeat_evset:
  if (PS_SUCCESS != ps_evset(evsetList_ptr, (char *)target_addr, EV_LLC,
                             evict_mem, HUGE_PAGES_AVAILABLE, thrDET))
    goto repeat_evset;

  printf("\nEviction set is constructed successfully\n\n");

  printf("\tEviction set addresses are: ");
  list_print(evsetList);


  // Convert the eviction set link-list to an array
  uint64_t evset[EV_LLC];
  list_to_array(evsetList, evset);

  // Set its first element as the scope_addr
  uint64_t scope_addr = evset[0];


  printf("\nTesting Prime+Scope\n\n");

  int access_time, success = 0;

#define TIME_READ_ACCESS(x) access_time = no_lfence_memaccesstime((void *)x)

  for (int t = 0; t < TEST_LEN; t++) {

    PRIME(evset);
    TIME_READ_ACCESS(scope_addr);
    TIME_READ_ACCESS(scope_addr);


    PRIME(evset);
    TIME_READ_ACCESS(scope_addr);   // SCOPE
    TIME_READ_ACCESS(scope_addr);   // SCOPE
    memaccess((void *)target_addr); // Cross-core access to monitored set
    TIME_READ_ACCESS(scope_addr);   // SCOPE detects access

    if (access_time > thrLLC) {
      success++;
      printf("#\t Success at test %d\n\n", t);
    } else
      printf("#\t Failure at test %d\n\n", t);
  }

  printf("\tSuccess %d/%d\n", success, TEST_LEN);
}
