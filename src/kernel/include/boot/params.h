#ifndef __BOOT_PARAMS_H__
#define __BOOT_PARAMS_H__

#include <libc/stdint.h>

typedef struct {
  uint64_t begin;
  uint64_t length;
  uint32_t type;
  uint32_t acpi;
} memory_region_t;

typedef struct {
  int region_count;
  memory_region_t *regions;
} memory_map_t;

typedef struct {
  memory_map_t memory;
  uint8_t boot_device;
} boot_params_t;

#endif