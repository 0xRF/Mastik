#pragma once

#include <stdint.h>

typedef struct threshold_info_t {
  uint32_t threshold;
  uint32_t lastlevelCache;
  uint32_t lowerCaches;
  uint32_t ram;

} threshold_info;

