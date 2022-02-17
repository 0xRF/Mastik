#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mastik/impl.h>
#include <mastik/info.h>
#include <mastik/list.h>
#include <mastik/list_traverse.h>
#include <mastik/low.h>
#include <mastik/ps.h>
#include <mastik/thresholds.h>

struct ps {
  int testMethod;
  struct lxinfo info;
  uintptr_t *guessPoolBuffer;
  uint32_t poolSize;
  threshold_info thresholds;
  uint32_t targetSetLength;
  uint32_t maxAttempts;
  uint32_t maxExtension;
  uint32_t pageSize;
};

// TODO dont be lazy
void l3info_init(lxinfo_t info) {
  info->associativity = 16;
  cpuid(&info->cpuidInfo);
  info->flags = LXFLAG_NOHUGEPAGES;
  info->slices = 1 << 3;
  info->sets = 1 << 10;
}

ps_t ps_prepare(int test_method, threshold_info *thresholds, l3info_t info) {

  ps_t rv = (ps_t)malloc(sizeof(struct ps));
  rv->testMethod = test_method;

  if (info != NULL)
    bcopy(&rv->info, info, sizeof(struct lxinfo));
  l3info_init(&rv->info);

  // TODO
  rv->maxAttempts = 32;
  rv->maxExtension = 48;
  rv->targetSetLength = 16;
  memcpy(&rv->thresholds, thresholds, sizeof(threshold_info));

  puts("Using thresholds:");
  printf("\t LLC %d", rv->thresholds.lastlevelCache);
  printf("\t L1/L2 %d", rv->thresholds.lowerCaches);
  printf("\t Threshold %d\n", rv->thresholds.threshold);
  rv->pageSize =
      ((rv->info.flags & LXFLAG_NOHUGEPAGES) == rv->info.flags) ? 4096 : 0;

  rv->guessPoolBuffer = (uintptr_t *)malloc(EVICT_LLC_SIZE);
  rv->poolSize = MAX_POOL_SIZE_SMALL;

  ps_premap(rv);

  return rv;
}

void ps_guess_pool_refresh(ps_t ps, uintptr_t *guess_pool, uintptr_t victim) {
  uintptr_t *page = ps->guessPoolBuffer;
  for (int i = 0; i < MAX_POOL_SIZE_SMALL; i++)
    guess_pool[i] =
        ((uint64_t)page + ((uint64_t)victim & (SMALLPAGE_PERIOD - 1)) +
         (rand() % MAX_POOL_SIZE_SMALL) * SMALLPAGE_PERIOD);
}

// int ps_evset_reduce(list head, char *victim, int len, int threshold);

static uint64_t guess_pool[MAX_POOL_SIZE];

int ps_set_create(ps_t ps, uintptr_t target, uint64_t *page, list evset) {
  int list_len = 0;
  int len = ps->targetSetLength;
  int guess_index = 0;
  int counter_attempt = 0;
  int threshold = ps->thresholds.threshold;

  *evset = NULL;

  int pool_size = ps->poolSize;

  // ps_guess_pool_refresh(ps, guess_pool, target);
  for (int i = 0; i < pool_size; i++) {
    guess_pool[i] = ((uint64_t)page + ((uint64_t)target & (SMALLPAGE_PERIOD - 1)) + (rand() % MAX_POOL_SIZE_SMALL) * SMALLPAGE_PERIOD);
  }

extend:
  while (LIKELY(list_len < len)) {
    int counter_guess = 0;
    int try_guesses = true;
    memaccess((void *)target);
    asm volatile("lfence;mfence");

    traverse_zigzag_victim(*evset, (void *)target);
    traverse_zigzag_victim(*evset, (void *)target);
    while (try_guesses) {

      node *candidate= (node *)guess_pool[guess_index];
      memaccess((void *)candidate);
      asm volatile("lfence");

      // Measure TARGET
     uint32_t time = no_lfence_memaccesstime((void *)target);

      if (UNLIKELY(time > threshold - 20 && time < 2 * threshold)) {
        try_guesses = false;
        counter_attempt = 0;

        // Add the guess to linkedlist
        list_len = list_append(evset, candidate);

        // Potential improvement: linked list for easy removal
        for (int i = guess_index; i < pool_size - 1; i++) {
          guess_pool[i] = guess_pool[i + 1];
        }
        pool_size--;
      } else {
        guess_index++;
      }

      // Wrap around the pool
      if (guess_index >= pool_size - 1) {
        guess_index = 0;
        try_guesses = false; // reinstate victim to be sure
        if (++counter_attempt >=
            MAX_ATTEMPT) { // If too many wrap-arounds, return with fail
          return PS_FAIL_CONSTRUCTION;
        }
      }
    }
  }

  if (LIKELY(!ps_set_valid(ps, *evset, target, threshold))) {
    if (LIKELY(++len < MAX_EXTENSION))
      goto extend; // Obtain one more address
    return PS_FAIL_EXTENSION;
  }

  if (list_len > ps->targetSetLength) {
    if (!ps_evset_reduce(ps, evset, list_len, ps->targetSetLength, target, threshold)) {
      return PS_FAIL_REDUCTION;
    }
  }

  return PS_SUCCESS;
}

void ps_premap(ps_t ps) {
    memset(ps->guessPoolBuffer, 1, EVICT_LLC_SIZE);
    memset(ps->guessPoolBuffer, 0, EVICT_LLC_SIZE);
}
int ps_evset_reduce(ps_t ps, list evset, int list_len, int targetLength,
                    uintptr_t target, uint32_t threshold) {

  for (int i = 0; i < list_len; i++) {

    node *popped = list_pop(evset);

    if (LIKELY(ps_set_valid(ps, *evset, target, threshold))) {
      if (list_length(*evset) == targetLength) {
        return 1;
      }
    } else {
      list_append(evset, popped);
    }
  }

  return 0;
}
static inline int comp(const void *a, const void *b) {
  return (*(uint64_t *)a - *(uint64_t *)b);
}

int ps_set_valid(ps_t ps, node* ptr, uintptr_t victim, uint32_t threshold) {

  int time[10], time_acc = 0;
  int i = 0;

  // Place TARGET in LLC
  asm volatile("lfence;mfence");
  memaccess((void *)victim);
  asm volatile("lfence;mfence");

  for (int test = 0; test < 10; test++) {

    // Potential improvement: could be sped up
    traverse_list_asm_skylake(ptr);
    traverse_list_asm_skylake(ptr);
    traverse_list_asm_skylake(ptr);
    traverse_list_asm_skylake(ptr);
    asm volatile("lfence;mfence");

    // Measure TARGET (and place in LLC for next iteration)
    time[test] = no_lfence_memaccesstime((void *)victim);
  }
  qsort(time, 10, sizeof(int), comp);

  return time[5] > threshold;
}

uint64_t *ps_get_guess_pool(ps_t ps) { return ps->guessPoolBuffer; }

inline int ps_scope(ps_t ps, uintptr_t scope, uint32_t threshold) {
  return no_lfence_memaccesstime((void *)scope) > threshold;
}
