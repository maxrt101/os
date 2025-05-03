#pragma once

#include <memory/memmap.h>
#include <stddef.h>

typedef struct kpblock_t {
  struct kpblock_t * prev;
  struct kpblock_t * next;
  size_t size;
} kpblock_t;

typedef struct {
  kpblock_t * root;
} kpalloc_t;

void kpmap(memmap_t * memmap);
uintptr_t kpalloc(size_t pages);
void kpfree(uintptr_t ptr, size_t pages);
void kpdump();