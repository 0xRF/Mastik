#include <mastik/prime.h>
#include <mastik/list.h>

// 2018 | Coffee Lake  | Inclusive LLC with 12 Ways | 4 Repeats
void traverse_Intel_Core_i7_9700K(uint64_t* arr) {
  int i;
  for(i=0; i<9; i+=4) {
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
  }
}

// 2017 | Kaby Lake    | Inclusive LLC with 16 Ways | 2 Repeats
void traverse_Intel_Core_i7_7700K(uint64_t* arr) {
  int i;
  for(i=0; i<13; i+=4) {
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
  }
}

// 2017 | Kaby Lake    | Inclusive LLC with 12 Ways | 3 Repeats
void traverse_Intel_Core_i5_7500 (uint64_t* arr) {
  int i;
  for(i=0; i<9; i+=4) {
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
  }
}

// 2015 | Skylake      | Inclusive LLC with 16 Ways | 3 Repeats
void traverse_Intel_Core_i7_6700 (uint64_t* set) {
  int i;
  for(i=0; i<13; i+=4) {
    memaccess((void *) set[i+0]);
    memaccess((void *) set[i+1]);
    memaccess((void *) set[  0]);
    memaccess((void *) set[  0]);
    memaccess((void *) set[i+2]);
    memaccess((void *) set[  0]);
    memaccess((void *) set[  0]);
    memaccess((void *) set[i+3]);
    memaccess((void *) set[i+0]);
    memaccess((void *) set[i+1]);
    memaccess((void *) set[i+2]);
    memaccess((void *) set[i+3]);
    memaccess((void *) set[i+0]);
    memaccess((void *) set[i+1]);
    memaccess((void *) set[i+2]);
    memaccess((void *) set[i+3]);
  }
}

// 2015 | Skylake      | Inclusive LLC with 12 Ways | 4 Repeats
void traverse_Intel_Core_i5_6500 (uint64_t* arr) {
  int i;
  for(i=0; i<9; i+=4) {
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
  }
}

// 2013 | Haswell      | Inclusive LLC with 16 Ways | 3 Repeats
void traverse_Intel_Core_i7_4790 (uint64_t* arr) {
  int i;
  for(i=0; i<13; i+=4) {
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+3]);
  }
}

// 2013 | Haswell      | Inclusive LLC with 12 Ways | 2 Repeats
void traverse_Intel_Core_i5_4590 (uint64_t* arr) {
  int i;
  for(i=0; i<10; i+=1) {
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
  }
}

// 2012 | Ivy Bridge   | Inclusive LLC with 16 Ways | 3 Repeats
void traverse_Intel_Core_i7_3770 (uint64_t* arr) {
  int i;
  for(i=0; i<13; i+=4) {
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+3]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
  }
}

// 2012 | Ivy Bridge   | Inclusive LLC with 12 Ways | 2 Repeats
void traverse_Intel_Core_i5_3450 (uint64_t* arr) {
  int i;
  for(i=0; i<10; i+=1) {
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
  }
}

// 2011 | Sandy Bridge | Inclusive LLC with 12 Ways | 5 Repeats
void traverse_Intel_Core_i5_2400 (uint64_t* arr) {
  int i;
  for(i=0; i<10; i+=1) {
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[  0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
    memaccess((void *) arr[i+0]);
    memaccess((void *) arr[i+1]);
    memaccess((void *) arr[i+2]);
  }
}

// 2019 | Xeon Silver  | Non-Inclusive LLC with 12 Ways | 1 Repeat
void traverse_Intel_Xeon_Silver_4208(node *list)
{
  // Fences at every two accesses against hardware stride prefetching [?]
	__asm__ volatile
	(		
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		"mfence;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rcx;"
		: // no output
		: "c" (list)
		: "cc", "memory"
	);
}

