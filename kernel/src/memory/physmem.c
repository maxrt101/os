#include <memory/physmem.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/util.h>
#include <kernel.h>

#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(addr) (((addr) / PAGE_SIZE) * PAGE_SIZE)
#define PAGE_ALIGN_UP(addr)   ((((addr) + PAGE_SIZE-1) / PAGE_SIZE) * PAGE_SIZE)

__STATIC_INLINE void kpblock_init(kpblock_t * root) {
  root->prev = root;
  root->next = root;
}

__STATIC_INLINE void kpblock_insert(kpblock_t ** root, kpblock_t * new_block) {
  kpblock_t * rb = *root;

  new_block->prev = rb->prev;
  new_block->next = rb;
  new_block->prev->next = new_block;
  new_block->next->prev = new_block;

  *root = new_block;
}

__STATIC_INLINE void kpblock_remove(kpblock_t ** root, kpblock_t * block) {
  block->prev->next = block->next;
  block->next->prev = block->prev;

  *root = block->prev;
}

void kpmap(memmap_t * memmap) {
  ASSERT_RETURN(memmap);

  kernel.kpalloc.root = NULL;

  struct {
    size_t blocks;
    size_t size;
  } stat = {0, 0};

  for (size_t i = 0; i < memmap->count; ++i) {
    if (memmap->regions[i].type == MEMREG_TYPE_USABLE) {
#if KPALLOC_VERBOSITY >= 2
      kprintf("kpalloc: adding region %x %x\n", memmap->regions[i].base, memmap->regions[i].size);
#endif
      if (kernel.kpalloc.root) {
        kpblock_insert(&kernel.kpalloc.root, (kpblock_t *) PAGE_ALIGN_UP(memmap->regions[i].base + kernel.hhdm.offset));
      } else {
        kernel.kpalloc.root = (kpblock_t *) PAGE_ALIGN_UP(memmap->regions[i].base + kernel.hhdm.offset);
        kpblock_init(kernel.kpalloc.root);
      }
      ((kpblock_t *) (PAGE_ALIGN_UP(memmap->regions[i].base) + kernel.hhdm.offset))->size = PAGE_ALIGN_DOWN(memmap->regions[i].size) / PAGE_SIZE;

      stat.blocks += 1;
      stat.size += PAGE_ALIGN_DOWN(memmap->regions[i].size);
    }
  }

  kprintf("kpalloc: %d blocks %d pages\n", stat.blocks, stat.size / PAGE_SIZE);
}

uintptr_t kpalloc(size_t pages) {
  ASSERT_RETURN(pages, 0);

  kpblock_t * block = kernel.kpalloc.root;
  do {
    if (block->size >= pages) {
      kpblock_remove(&kernel.kpalloc.root, block);
      if (block->size > pages) {
        kpblock_t * new_block = (kpblock_t *) PAGE_ALIGN_UP((uint64_t) block + pages * PAGE_SIZE);
        kpblock_insert(&kernel.kpalloc.root, new_block);
        new_block->size = block->size - pages;
#if KPALLOC_VERBOSITY >= 3
        kprintf("kpalloc: remove %d at %p add %d at %p\n", block->size, block, new_block->size, new_block);
#endif
      }
#if KPALLOC_VERBOSITY >= 1
      kprintf("kpalloc: allocated %d pages at %p\n", pages, block);
#endif
      memset(block, 0, PAGE_SIZE * pages);
      return (uintptr_t) block - kernel.hhdm.offset;
    }
    block = block->next;
  } while (block != kernel.kpalloc.root);

  kprintf("kpalloc: Can't allocate %d pages - no memory\n", pages);
  return 0;
}

void kpfree(uintptr_t ptr, size_t pages) {
  // TODO: Defragnent
  kpblock_t * block = (kpblock_t *)(ptr + kernel.hhdm.offset);
  kpblock_insert(&kernel.kpalloc.root, block);
  block->size = pages;
#if KPALLOC_VERBOSITY >= 1
  kprintf("kpalloc: freed %d pages at %p\n", pages, ptr);
#endif
}

void kpdump() {
  kpblock_t * block = kernel.kpalloc.root;
  do {
    kprintf("kpdump: %d pages free at %p\n", block->size, block);
    block = block->next;
  } while (block != kernel.kpalloc.root);
}
