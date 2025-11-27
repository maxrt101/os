#pragma once

#include <stdint.h>

/**
 *
 * void kpmap(memmap_t * memmap);
 * uintptr_t kpalloc(size_t pages);
 * void kpfree(uintptr_t ptr, size_t pages);
 * void kpdump();
 *
 * kpmap
 * kpalloc
 * kpfree
 * kpdump
 *
 * kernel_phys_map()
 * kernel_phys_alloc()
 * kernel_phys_free()
 * kernel_phys_dump()
 *
 * kernel_virt_map()
 * kernel_virt_map_page()
 * kernel_virt_unmap_page()
 * kernel_virt_translate()
 *
 */

void kernel_virt_map(void);
void kernel_virt_map_page(void * vaddr, uint64_t paddr, uint64_t flags);
void kernel_virt_unmap_page(void * vaddr);
uint64_t kernel_virt_translate(void * vaddr);
