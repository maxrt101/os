#include <memory/physmem.h>
#include <memory/memory.h>
#include <util/string.h>
#include <util/assert.h>
#include <util/util.h>
#include <kernel.h>

__STATIC_INLINE void kernel_phys_block_init(kernel_phys_block_t * root) {
  root->prev = root;
  root->next = root;
}

__STATIC_INLINE void kernel_phys_block_insert(kernel_phys_block_t ** root, kernel_phys_block_t * new_block) {
  kernel_phys_block_t * rb = *root;

  new_block->prev = rb->prev;
  new_block->next = rb;
  new_block->prev->next = new_block;
  new_block->next->prev = new_block;

  *root = new_block;
}

__STATIC_INLINE void kernel_phys_block_remove(kernel_phys_block_t ** root, kernel_phys_block_t * block) {
  block->prev->next = block->next;
  block->next->prev = block->prev;

  *root = block->prev;
}

__STATIC_INLINE void phys_map_region(uint64_t base, uint64_t size) {
  // Align the base and size to page boundaries
  uint64_t aligned_base = PAGE_ALIGN_UP(base);
  uint64_t aligned_size = PAGE_ALIGN_DOWN(size);
  size_t pages = aligned_size / PAGE_SIZE;

  if (pages == 0) return;

  // Calculate the Virtual Address (HHDM)
  kernel_phys_block_t * new_block = (kernel_phys_block_t *)(aligned_base + kernel.mem.hhdm);

  // If root exists, insert. If root is NULL, initialize it.
  if (kernel.mem.phys.root) {
    kernel_phys_block_insert(&kernel.mem.phys.root, new_block);
  } else {
    kernel.mem.phys.root = new_block;
    kernel_phys_block_init(kernel.mem.phys.root);
  }

  // Set the size
  new_block->size = pages;

#if KPALLOC_VERBOSITY >= 0
  kprintf("phys: added region %x (pages: %d)\n", aligned_base, pages);
#endif
}

void kernel_phys_map(memmap_t * memmap) {
  ASSERT_RETURN(memmap);

  kernel.mem.phys.root = NULL;

  kprintf("phys: Mapping physical memory\n");

  struct {
    size_t blocks;
    size_t size;
  } stat = {0, 0};

  for (size_t i = 0; i < memmap->count; ++i) {
    if (memmap->regions[i].type == MEMREG_TYPE_USABLE) {
      phys_map_region(memmap->regions[i].base, memmap->regions[i].size);

      stat.blocks += 1;
      stat.size += PAGE_ALIGN_DOWN(memmap->regions[i].size);
    }
  }

  kprintf("phys: mapped %d blocks, %d pages\n", stat.blocks, stat.size / PAGE_SIZE);
}

uintptr_t kernel_phys_alloc(size_t pages) {
  ASSERT_RETURN(pages, 0);

  kernel_phys_block_t * block = kernel.mem.phys.root;
  do {
    if (block->size >= pages) {
      kernel_phys_block_remove(&kernel.mem.phys.root, block);
      if (block->size > pages) {
        kernel_phys_block_t * new_block = (kernel_phys_block_t *) PAGE_ALIGN_UP((uint64_t) block + pages * PAGE_SIZE);
        kernel_phys_block_insert(&kernel.mem.phys.root, new_block);
        new_block->size = block->size - pages;
#if KPALLOC_VERBOSITY >= 3
        kprintf("phys: remove %d at %p add %d at %p\n", block->size, block, new_block->size, new_block);
#endif
      }
#if KPALLOC_VERBOSITY >= 1
      kprintf("phys: allocated %d pages at %p\n", pages, block);
#endif
      memset(block, 0, PAGE_SIZE * pages);
      return (uintptr_t) block - kernel.mem.hhdm;
    }
    block = block->next;
  } while (block != kernel.mem.phys.root);

  kprintf("phys: Can't allocate %d pages - no memory\n", pages);
  return 0;
}

void kernel_phys_free(uintptr_t ptr, size_t pages) {
  // TODO: Defragment
  kernel_phys_block_t * block = (kernel_phys_block_t *)(ptr + kernel.mem.hhdm);
  kernel_phys_block_insert(&kernel.mem.phys.root, block);
  block->size = pages;
#if KPALLOC_VERBOSITY >= 1
  kprintf("phys: freed %d pages at %p\n", pages, ptr);
#endif
}

void kernel_phys_dump() {
  kernel_phys_block_t * block = kernel.mem.phys.root;
  do {
    kprintf("phys: %d pages free at %p\n", block->size, block);
    block = block->next;
  } while (block != kernel.mem.phys.root);
}
