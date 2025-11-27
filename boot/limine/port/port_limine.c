#include <util/util.h>
#include <kernel.h>
#include <arch.h>
#include <limine.h>
#include <stdbool.h>
#include <stddef.h>

#define MEMREG_MAX 32

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
  .id = LIMINE_MEMMAP_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kernel_address_request = {
  .id = LIMINE_KERNEL_ADDRESS_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

__STATIC_INLINE memreg_type_t limine_memmap_entry_type_to_memreg_type(uint64_t type) {
  switch (type) {
    case LIMINE_MEMMAP_USABLE:                  return MEMREG_TYPE_USABLE;
    case LIMINE_MEMMAP_RESERVED:                return MEMREG_TYPE_RESERVED;
    case LIMINE_MEMMAP_ACPI_RECLAIMABLE:        return MEMREG_TYPE_ACPI_RECLAIMABLE;
    case LIMINE_MEMMAP_ACPI_NVS:                return MEMREG_TYPE_ACPI_NVS;
    case LIMINE_MEMMAP_BAD_MEMORY:              return MEMREG_TYPE_BAD_MEMORY;
    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:  return MEMREG_TYPE_BOOTLOADER_RECLAIMABLE;
    case LIMINE_MEMMAP_KERNEL_AND_MODULES:      return MEMREG_TYPE_KERNEL;
    case LIMINE_MEMMAP_FRAMEBUFFER:             return MEMREG_TYPE_FRAMEBUFFER;
    default:                                    return MEMREG_TYPE_RESERVED;
  }
}

static memreg_t memory_regions[MEMREG_MAX];

void kernel_init_port(kernel_t * kernel) {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    arch_abort();
  }

  if (hhdm_request.response == NULL || hhdm_request.response->offset == 0) {
    arch_abort();
  }

  kernel->mem.hhdm = hhdm_request.response->offset;

  if (kernel_address_request.response == NULL) {
    arch_abort();
  }

  kernel->mem.kernel_phys_base = kernel_address_request.response->physical_base;
  kernel->mem.kernel_virt_base = kernel_address_request.response->virtual_base;

  if (memmap_request.response == NULL || memmap_request.response->entry_count < 1) {
    arch_abort();
  }

  kernel->mem.map.count = memmap_request.response->entry_count;
  kernel->mem.map.regions = memory_regions;

  for (size_t i = 0; i < kernel->mem.map.count; ++i) {
    memory_regions[i].base = memmap_request.response->entries[i]->base;
    memory_regions[i].size = memmap_request.response->entries[i]->length;
    memory_regions[i].type = limine_memmap_entry_type_to_memreg_type(memmap_request.response->entries[i]->type);
  }

  if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
    // TODO: Use fallback - VGA char mode (or VGA framebuffer?)
    arch_abort();
  }

  struct limine_framebuffer * framebuffer = framebuffer_request.response->framebuffers[0];

  framebuffer_t * fb = &kernel->framebuffer;

  fb->address           = framebuffer->address;
  fb->size.height       = framebuffer->height;
  fb->size.width        = framebuffer->width;
  fb->info.bpp          = framebuffer->bpp;
  fb->info.pitch        = framebuffer->pitch;
  fb->info.width_bytes  = framebuffer->width * framebuffer->bpp / 8;
  fb->info.color_order  = COLOR_ORDER_ARGB;
}
