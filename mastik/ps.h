#pragma once

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
#define MAX_EXTENSION  32
#define MAX_RETRY 32
#define MAX_ATTEMPT 32
#define LLC_WAYS 16


#include <mastik/list.h>
#include <stdint.h>

int  ps_evset       (list evset, char *victim, int len, uint64_t* page, int is_huge, int threshold);
int  ps_evset_test  (list evset, char *victim, int threshold, int test_len, int test_method);
int  ps_evset_premap(uint64_t* page);
