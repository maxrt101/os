#pragma once

#include <memory/memmap.h>
#include <stddef.h>

typedef struct kpblock_t {
  struct kpblock_t * prev;
  struct kpblock_t * next;
  size_t size;
} kernel_phys_block_t;

typedef struct {
  kernel_phys_block_t * root;
} kernel_phys_alloc_t;

void kernel_phys_map(memmap_t * memmap);
uintptr_t kernel_phys_alloc(size_t pages);
void kernel_phys_free(uintptr_t ptr, size_t pages);
void kernel_phys_dump();