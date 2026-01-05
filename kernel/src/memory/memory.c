#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <util/string.h>
#include "memory/physmem.h"
#include "memory/virtmem.h"
#include "memory/memory.h"
#include "kernel.h"
#include "util/assert.h"

// Define where the heap starts in Virtual Memory
// This must be a free area in High Half map
// 0xffffffff90000000 is after kernel code
#define KHEAP_START_VIRT   0xFFFFFFFF90000000
#define KHEAP_INITIAL_SIZE (4 * 4096) // Start with 4 pages (16KB)
#define KHEAP_MIN_SIZE     32         // Min alloc size to prevents fragmentation

typedef struct __PACKED kheap_block_t {
  struct kheap_block_t * next;
  size_t size; // Total size (including header)
  bool is_free;
} kheap_block_t;

static struct {
  kheap_block_t * head;
  uint64_t current_end_virt;
} kheap;

__STATIC_INLINE size_t align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

static bool kheap_extend(size_t size) {
  size_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;

  for (size_t i = 0; i < pages_needed; i++) {
    uint64_t phys = kernel_phys_alloc(1);
    if (!phys) {
      kprintf("kheap: OOM during extension!\n");
      return false;
    }

    // Map to the next available virtual address
    void *virt = (void*)kheap.current_end_virt;
    kernel_virt_map_page(kernel.arch.page_table, virt, phys);

    // Zero the memory
    memset(virt, 0, PAGE_SIZE);

    kheap.current_end_virt += PAGE_SIZE;
  }

  // Create a new block at the end of the list
  // Since we map contiguously, the old end is the start of the new block
  // We need to find the last block in the list to link it

  kheap_block_t *tail = kheap.head;
  while (tail->next != NULL) {
    tail = tail->next;
  }

  // Convert the new memory area into a block
  // Note: In a smarter implementation, we would try to merge with 'tail'
  // immediately if 'tail' was free. For simplicity, we just append.

  uint64_t total_new_size = pages_needed * PAGE_SIZE;
  kheap_block_t *new_block = (kheap_block_t *)((uint8_t*)tail + tail->size);

  // Let's assume the previous tail ended exactly where the new page starts.
  new_block->is_free = true;
  new_block->size = total_new_size;
  new_block->next = NULL;

  tail->next = new_block;

  // Coalesce immediately just in case
  if (tail->is_free) {
    tail->size += new_block->size;
    tail->next = new_block->next;
  }

  return true;
}

void kheap_init(void) {
  kheap.current_end_virt = KHEAP_START_VIRT;

  // Allocate 4 pages manually
  for (int i = 0; i < 4; i++) {
    uint64_t phys = kernel_phys_alloc(1);
    kernel_virt_map_page(kernel.arch.page_table, (void*)kheap.current_end_virt, phys);
    kheap.current_end_virt += PAGE_SIZE;
  }

  // Create the Root Block
  kheap.head = (kheap_block_t *)KHEAP_START_VIRT;
  kheap.head->size = (4 * PAGE_SIZE); // Total size
  kheap.head->is_free = true;
  kheap.head->next = NULL;

  kprintf("kheap: initialized at %p size %d\n", kheap.head, kheap.head->size);
}

void * kmalloc(size_t size) {
  ASSERT_RETURN(size, NULL);

  size_t aligned_size = align_up(size, 16);
  size_t total_needed = aligned_size + sizeof(kheap_block_t);

  kheap_block_t *curr = kheap.head;

  while (curr) {
    if (curr->is_free && curr->size >= total_needed) {
      if (curr->size >= total_needed + sizeof(kheap_block_t) + KHEAP_MIN_SIZE) {
        size_t remaining_size = curr->size - total_needed;

        kheap_block_t *new_next = (kheap_block_t *)((uint8_t*)curr + total_needed);
        new_next->is_free = true;
        new_next->size = remaining_size;
        new_next->next = curr->next;

        curr->size = total_needed;
        curr->next = new_next;
      }

      curr->is_free = false;
      return (void*)((uint8_t*)curr + sizeof(kheap_block_t));
    }

    // If we reached the end and didn't find space
    if (curr->next == NULL) {
      if (kheap_extend(total_needed)) {
        curr = curr->next ? curr->next : curr;
        continue;
      }

    return NULL;
    }

    curr = curr->next;
  }
  return NULL;
}

void kfree(void * ptr) {
  ASSERT_RETURN(ptr);

  kheap_block_t *block = (kheap_block_t *)((uint8_t*)ptr - sizeof(kheap_block_t));
  block->is_free = true;

  // Defrag
  kheap_block_t *curr = kheap.head;
  while (curr) {
    if (curr->is_free && curr->next && curr->next->is_free) {
      curr->size += curr->next->size;
      curr->next = curr->next->next;
      // Don't advance 'curr', try to merge again with the new next
    } else {
      curr = curr->next;
    }
  }
}
