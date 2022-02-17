#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <mastik/l3.h>
#include <mastik/list.h>
#include <mastik/list_traverse.h>
#include <mastik/low.h>
#include <mastik/prime.h>
#include <mastik/ps.h>
#include <mastik/util.h>

double time_diff_ms(struct timespec begin, struct timespec end) {
  double timespan;
  if ((end.tv_nsec - begin.tv_nsec) < 0) {
    timespan = (end.tv_sec - begin.tv_sec - 1) * 1.0e3;
    timespan += (end.tv_nsec - begin.tv_nsec + 1000000000UL) * 1.0e-6;
  } else {
    timespan = (end.tv_sec - begin.tv_sec) * 1.0e3;
    timespan += (end.tv_nsec - begin.tv_nsec) * 1.0e-6;
  }
  return timespan;
}

int has_privilege() {
  uid_t uid = getuid(), euid = geteuid();
  return (euid <= 0 || uid != euid);
}

// #define VICTIM_READ_ACCESS(x)   ({                                \
//   *synchronization_params = (volatile uint64_t)x;                  \
//   *synchronization = 11;                                           \
//   while(*synchronization==11);                                     })
//

#define PAGE_BITS 12
#define LINE_BITS 6
#define SLICE_BITS 3
#define SET_BITS 10

#define PAGE_SIZE2 (1 << PAGE_BITS)
#define LINE_SIZE (1 << LINE_BITS)
#define CACHE_SLICES (1 << SLICE_BITS)
#define CACHE_SETS (1 << SET_BITS)

unsigned long long vtop(unsigned pid, unsigned long long vaddr) {
  char path[1024];
  sprintf(path, "/proc/self/pagemap", pid);
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  unsigned long long paddr = -1;
  unsigned long long index = (vaddr / PAGE_SIZE2) * sizeof(paddr);
  if (pread(fd, &paddr, sizeof(paddr), index) != sizeof(paddr)) {
    return -1;
  }
  close(fd);
  paddr &= 0x7fffffffffffff;
  return (paddr << PAGE_BITS) | (vaddr & (PAGE_SIZE2 - 1));
}

unsigned int count_bits(unsigned long long n) {
  unsigned int count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}

unsigned int nbits(unsigned long long n) {
  unsigned int ret = 0;
  n = n >> 1;
  while (n > 0) {
    n >>= 1;
    ret++;
  }
  return ret;
}

unsigned long long ptos(unsigned long long paddr, unsigned long long bits) {
  unsigned long long ret = 0;
  unsigned long long mask[3] = {0x1b5f575440ULL, 0x2eb5faa880ULL,
                                0x3cccc93100ULL}; // according to Maurice et al.
  switch (bits) {
  case 3:
    ret = (ret << 1) | (unsigned long long)(count_bits(mask[2] & paddr) % 2);
  case 2:
    ret = (ret << 1) | (unsigned long long)(count_bits(mask[1] & paddr) % 2);
  case 1:
    ret = (ret << 1) | (unsigned long long)(count_bits(mask[0] & paddr) % 2);
  default:
    break;
  }
  return ret;
}

void check(unsigned int pid, unsigned long long *virtual_addresses,
           unsigned int length) {
  unsigned int cache_sets = 1024;

  unsigned long long paddr = 0, cacheset = 0, slice = 0;

  for (unsigned int i = 0; i < length; i++) {
    paddr = vtop(pid, virtual_addresses[i]);
    cacheset = (paddr >> LINE_BITS) & (CACHE_SETS - 1);
    slice = ptos(paddr, SLICE_BITS);
    printf("cache set: 0x%llx, slice: 0x%llx\n", cacheset, slice);
  }
}

//
//
//
volatile uint64_t *shared_mem;
volatile uint64_t *synchronization;
volatile uint64_t *synchronization_params;

void victim() {
#define FENCE asm volatile("mfence\n\t lfence\n\t");

  while (1) {
    if (*synchronization == -1) {
      break;
    } else if (*synchronization == 11) {
      /* Implements the VICTIM_READ_ACCESS() macro */
      memaccess((void *)*synchronization_params);
      memaccess((void *)*synchronization_params);
      memaccess((void *)*synchronization_params);
      memaccess((void *)*synchronization_params);
      FENCE
      *synchronization = 0;
    }
  }

  exit(EXIT_SUCCESS);
}
#include <mastik/util.h>
int main() {

  shared_mem = mmap(NULL, 32 * 4096, (PROT_READ | PROT_WRITE),
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);

  void *mp = malloc(4096);
  // Initialize Shared Memory Between Process and Fork
  synchronization = mmap(NULL, sizeof(uint64_t), (PROT_READ | PROT_WRITE),
                         MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  synchronization_params = mmap(NULL, sizeof(uint64_t), PROT_READ | PROT_WRITE,
                                (MAP_SHARED | MAP_ANONYMOUS), 0, 0);

  if (*synchronization_params == MAP_FAILED || *synchronization == MAP_FAILED) {
    perror("Faild to map shared memory");
    return EXIT_FAILURE;
  }

  *synchronization = 0;
  *synchronization_params = 0;
  setaffinity(7);

  threshold_info thresholds = {
      .lowerCaches = 42, .lastlevelCache = 111, .ram = 250, .threshold = 203};

  struct l3info l3info;
  bzero(&l3info, sizeof(l3info));

  pid_t pid = fork();
  if (pid == 0) {
    // setaffinity(2);
    // victim();
    return 0;
  }
  kill(pid, SIGTERM);

  *synchronization = 0;

  ps_t ps = ps_prepare(0, &thresholds, &l3info);

#define EV_LLC LLC_WAYS

  node *evsetList;
  list evsetList_ptr = &evsetList;

  *evsetList_ptr = NULL;
  int count = 1000;
  // scanf("%d", &count);

  printf("running set construction %d times\n", count);
  uint64_t *page = ps_get_guess_pool(ps);
  lfence();
  mfence();
  uint64_t target_addr = 0;
  // clock_t t1 = clock();
  // for (int i = 0; i < count; i++) {
  // repeat_evset : {
  //   target_addr = (uint64_t)shared_mem + (rand() % 32) * 4096;
  //   if (PS_SUCCESS != ps_set_create(ps, target_addr, page, evsetList_ptr))
  //     goto repeat_evset;
  // }

  target_addr = (uint64_t)shared_mem + (rand() % 32) * 4096;
  double timespan;
  int attempt_counter = 0;
  struct timespec tstart = {0, 0}, tend = {0, 0};
for(int i = 0; i < 100; i++){
repeat_evset:
  clock_gettime(CLOCK_MONOTONIC, &tstart);
create:
  if(PS_SUCCESS != ps_set_create(ps, target_addr, page, evsetList_ptr))
      goto create;

  if (++attempt_counter < count) {
    clock_gettime(CLOCK_MONOTONIC, &tend);
    timespan += time_diff_ms(tstart, tend);
    goto repeat_evset;
  }
  printf("%f\n: ", timespan / count);
  timespan = 0;
  attempt_counter = 0;
}
  return 0;
  list_print(evsetList);

  // Convert the eviction set link-list to an array
  uintptr_t evset[EV_LLC];
  list_to_array(evsetList, evset);
  if (has_privilege())
    check(getpid(), (unsigned long long *)evset, 16);

  // Set its first element as the scope_addr
  uintptr_t scope = evset[0];

  printf("\nTesting Prime+Scope\n\n");

  int access_time, success = 0;
#define VICTIM_READ_ACCESS(x)                                                  \
  ({                                                                           \
    *synchronization_params = (volatile uint64_t)x;                            \
    *synchronization = 11;                                                     \
    while (*synchronization == 11)                                             \
      ;                                                                        \
  })

  for (int t = 0; t < TEST_LEN; t++) {

    PRIME(evset);

    // VICTIM_READ_ACCESS(target_addr);
    memaccess((void *)target_addr);

    if (ps_scope(ps, scope, thresholds.lastlevelCache))
      success++;
  }

  printf("\tSuccess %d/%d\n", success, TEST_LEN);

  return 0;
}
