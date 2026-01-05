#pragma once

#include <stdint.h>

#define KERNEL_PAGE_PRESENT 1 << 0
#define KERNEL_PAGE_RW      1 << 1

void kernel_virt_map(void);
void kernel_virt_map_page(void * table, void * vaddr, uint64_t paddr);
void kernel_virt_unmap_page(void * vaddr);
uint64_t kernel_virt_translate(void * vaddr);
