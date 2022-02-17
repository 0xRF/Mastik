#pragma once

#include "mastik/low.h"
#define MAX_POOL_SIZE_HUGE  (EVICT_LLC_SIZE/LLC_PERIOD)
#define MAX_POOL_SIZE_SMALL (EVICT_LLC_SIZE/SMALLPAGE_PERIOD) 
#define MAX_POOL_SIZE       (((MAX_POOL_SIZE_HUGE)>(MAX_POOL_SIZE_SMALL)) ? \
                              (MAX_POOL_SIZE_HUGE):(MAX_POOL_SIZE_SMALL)    \
                            )

////////////////////////////////////////////////////////////////////////////////
// Return Values

#define PS_SUCCESS                1
#define PS_FAIL_CONSTRUCTION     -1
#define PS_FAIL_EXTENSION        -3
#define PS_FAIL_REDUCTION        -4
#define PS_FAIL_CONTDIR_EVICTION -5

////////////////////////////////////////////////////////////////////////////////
// Declaration of types

#define EVTEST_MEAN     0
#define EVTEST_MEDIAN   1
#define EVTEST_ALLPASS  2

#define CONTENTION_LLC  0
#define CONTENTION_CD   1

#define WITH_HUGE       1
#define WITH_SMALL      0

#define SKYLAKE16
#define LLC_SLICES 8

#define HUGE_PAGES_AVAILABLE  0
#define MB 1024*1024

#define LLC_PERIOD 4096
#define SMALLPAGE_PERIOD 4096

#define EVICT_L1_SIZE         ( 8*MB)
#define EVICT_L2_SIZE         ( 8*MB)
#if LLC_SLICES > 8
  #define EVICT_LLC_SIZE        (512*MB) // especially the 28-slice machines need larger pool
#else
  #define EVICT_LLC_SIZE        (128*MB) // especially the 28-slice machines need larger pool
#endif
#define SHARED_MEM_SIZE       (128*MB)
 
#define TEST_LEN  10
#define MAX_EVSET_CONST_RETRY 25
#define RANDOMIZE_TARGET 1
#define RENEW_THRESHOLD 0
#define ENABLE_EARLY_EVICTION_CHECK 1
#define ENABLE_EXTENSION 1
#define ENABLE_REDUCTION 1
#define RANDOMIZE_GUESS_POOL 1
#define MAX_EXTENSION 48
#define MAX_RETRY 32
#define MAX_ATTEMPT 20
#define LLC_WAYS 16


#include <mastik/list.h>
#include <mastik/thresholds.h>
#include <stdint.h>
typedef struct ps *ps_t;
typedef struct l3info *l3info_t;

/// Initailize The attack context
/// done else where \param test_method \param info \return
ps_t ps_prepare(int test_method, threshold_info *thresholds, l3info_t info);


void ps_premap(ps_t ps);

void ps_guess_pool_refresh(ps_t ps, uintptr_t* guess_pool, uintptr_t target);

void ps_release(ps_t ps);
/// Creates a eviction set which is congruent to the target address
/// \param ps
/// \param target
/// \return 1 when a valid set was created
int ps_set_create(ps_t ps, uintptr_t target, uint64_t* page, list set);

uint64_t* ps_get_guess_pool(ps_t ps);

///
/// \param ps
/// \param set
/// \param victim
/// \return
int ps_set_valid(ps_t ps, node* ptr, uintptr_t victim, uint32_t threshold);

/// Reduce an eviction set
/// \param ps ctx
/// \param set a congruent set to the target address
/// \param target
/// \return 1 if the eviction set was reduced to targetSetLength
// int ps_set_reduce(ps_t ps, list set, int length, uintptr_t target);
int ps_evset_reduce(ps_t ps, list evset, int length, int targetLength, uintptr_t target, uint32_t threshold);

/// prime the eviction set prior, loading S (the first element) to be the
/// eviction candiate \param ps context \param set
 void ps_prime(ps_t ps, uintptr_t *set);

/// Scope the target address, this will measure the memeroy access time
/// \param ps context
/// \param set the eviction set to scope, will use the fist addresses in the set
/// as the 'scope address' \return 1 if the scope addr has been evicted from the
/// cache (slow memaccesses)
int ps_scope(ps_t ps, uintptr_t scope, uint32_t threshold);


