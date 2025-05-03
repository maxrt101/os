#include <memory/memmap.h>
#include <util/assert.h>
#include <kernel.h>
#include <stddef.h>

const char * memreg_type_to_str(memreg_type_t type) {
  switch (type) {
    case MEMREG_TYPE_USABLE:                  return "Usable";
    case MEMREG_TYPE_RESERVED:                return "Reserved";
    case MEMREG_TYPE_ACPI_RECLAIMABLE:        return "ACPI Reclaimable";
    case MEMREG_TYPE_ACPI_NVS:                return "ACPI NVS";
    case MEMREG_TYPE_BAD_MEMORY:              return "Bad Memory";
    case MEMREG_TYPE_BOOTLOADER_RECLAIMABLE:  return "Bootloader Reclaimable";
    case MEMREG_TYPE_KERNEL:                  return "Kernel";
    case MEMREG_TYPE_FRAMEBUFFER:             return "Framebuffer";
    default:                                  return "?";
  }
}

void memmap_dump(memmap_t * memmap) {
  ASSERT_RETURN(memmap);

  kprintf("Memory Map:\n");
  kprintf("Base               Size               Type\n");
  for (size_t i = 0; i < memmap->count; ++i) {
    kprintf("%016p %016p %s\n", memmap->regions[i].base, memmap->regions[i].size, memreg_type_to_str(memmap->regions[i].type));
  }
}
