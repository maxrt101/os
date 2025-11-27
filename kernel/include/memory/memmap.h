#pragma once

#include <stdint.h>

typedef enum {
  MEMREG_TYPE_USABLE = 0,
  MEMREG_TYPE_RESERVED,
  MEMREG_TYPE_ACPI_RECLAIMABLE,
  MEMREG_TYPE_ACPI_NVS,
  MEMREG_TYPE_BAD_MEMORY,
  MEMREG_TYPE_BOOTLOADER_RECLAIMABLE,
  MEMREG_TYPE_KERNEL,
  MEMREG_TYPE_FRAMEBUFFER,
} memreg_type_t;

typedef struct memreg_t {
  memreg_type_t type;
  uint64_t      base;
  uint64_t      size;
} memreg_t;

typedef struct memmap_t {
  uint64_t   count;
  memreg_t * regions;
} memmap_t;

const char * memreg_type_to_str(memreg_type_t type);
void memmap_dump(memmap_t * memmap);
