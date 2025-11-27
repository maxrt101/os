#include <memory/virtmem.h>
#include <memory/memmap.h>
#include <arch.h>

void kernel_virt_map(void) {
  arch_virt_map();
}

void kernel_virt_map_page(void * vaddr, uint64_t paddr, uint64_t flags) {
  // arch_virt_map_page(vaddr, paddr, flags);
}

void kernel_virt_unmap_page(void * vaddr) {
  // arch_virt_unmap_page(vaddr);
}

uint64_t kernel_virt_translate(void * vaddr) {
  // return arch_virt_translate(vaddr);
  return 0;
}

