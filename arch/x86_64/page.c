#include <stdint.h>
#include <stddef.h>
#include <util/string.h>
#include <kernel.h>
#include <x86_64.h>
#include <memory/physmem.h>

#define PAGE_SIZE    4096
#define PAGE_ENTRIES 512
#define PAGE_PRESENT 0x001
#define PAGE_RW      0x002
#define PAGE_USER    0x004

#define PML4_INDEX(addr) (((uint64_t)(addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((uint64_t)(addr) >> 30) & 0x1FF)
#define PD_INDEX(addr)   (((uint64_t)(addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)   (((uint64_t)(addr) >> 12) & 0x1FF)

#define P2V(paddr) ((void *)((uintptr_t)(paddr) + kernel.mem.hhdm))
#define V2P(vaddr) ((uintptr_t)(vaddr) - kernel.mem.hhdm)

__STATIC_FORCEINLINE void load_cr3(uint64_t phys) {
    ARCH_ASM("mov %0, %%cr3" :: "r"(phys) : "memory");
}

__STATIC_INLINE uint64_t read_cr3(void) {
    uint64_t value;
    ARCH_ASM("mov %%cr3, %0" : "=r"(value));
    return value;
}

static uint64_t* get_table_ptr(uint64_t entry) {
  uint64_t phys = entry & ~0xFFF;
  return (uint64_t*) P2V(phys);
}

void arch_virt_map_page(uint64_t * pml4, void *vaddr, uint64_t paddr) {
  uint16_t pml4_idx = PML4_INDEX(vaddr);
  uint16_t pdpt_idx = PDPT_INDEX(vaddr);
  uint16_t pd_idx   = PD_INDEX(vaddr);
  uint16_t pt_idx   = PT_INDEX(vaddr);

  if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
    uint64_t pdpt_phys = kernel_phys_alloc(1);
    memset(P2V(pdpt_phys), 0, PAGE_SIZE);
    pml4[pml4_idx] = pdpt_phys | PAGE_PRESENT | PAGE_RW | PAGE_USER;
  }

  uint64_t *pdpt = get_table_ptr(pml4[pml4_idx]);

  if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
    uint64_t pd_phys = kernel_phys_alloc(1);
    memset(P2V(pd_phys), 0, PAGE_SIZE);
    pdpt[pdpt_idx] = pd_phys | PAGE_PRESENT | PAGE_RW | PAGE_USER;
  }

  // Get Page Directory
  uint64_t *pd = get_table_ptr(pdpt[pdpt_idx]);

  if (!(pd[pd_idx] & PAGE_PRESENT)) {
    uint64_t pt_phys = kernel_phys_alloc(1);
    memset(P2V(pt_phys), 0, PAGE_SIZE);
    pd[pd_idx] = pt_phys | PAGE_PRESENT | PAGE_RW | PAGE_USER;
  }

  uint64_t *pt = get_table_ptr(pd[pd_idx]);

  pt[pt_idx] = (paddr & ~0xFFF) | PAGE_PRESENT | PAGE_RW;
}

void arch_virt_unmap_page(void *vaddr) {
  uintptr_t addr = (uintptr_t)vaddr;
  uint64_t cr3 = read_cr3();

  uint64_t *pml4 = P2V(cr3);
  if (!(pml4[PML4_INDEX(addr)] & PAGE_PRESENT)) return;

  uint64_t *pdpt = P2V(pml4[PML4_INDEX(addr)] & ~0xFFFULL);
  if (!(pdpt[PDPT_INDEX(addr)] & PAGE_PRESENT)) return;

  uint64_t *pd = P2V(pdpt[PDPT_INDEX(addr)] & ~0xFFFULL);
  if (!(pd[PD_INDEX(addr)] & PAGE_PRESENT)) return;

  uint64_t *pt = P2V(pd[PD_INDEX(addr)] & ~0xFFFULL);
  if (!(pt[PT_INDEX(addr)] & PAGE_PRESENT)) return;

  pt[PT_INDEX(addr)] = 0;
}

uint64_t arch_virt_translate(void *vaddr) {
  uintptr_t addr = (uintptr_t)vaddr;
  uint64_t cr3 = read_cr3();

  uint64_t *pml4 = P2V(cr3);
  if (!(pml4[PML4_INDEX(addr)] & PAGE_PRESENT)) return 0;

  uint64_t *pdpt = P2V(pml4[PML4_INDEX(addr)] & ~0xFFFULL);
  if (!(pdpt[PDPT_INDEX(addr)] & PAGE_PRESENT)) return 0;

  uint64_t *pd = P2V(pdpt[PDPT_INDEX(addr)] & ~0xFFFULL);
  if (!(pd[PD_INDEX(addr)] & PAGE_PRESENT)) return 0;

  uint64_t *pt = P2V(pd[PD_INDEX(addr)] & ~0xFFFULL);
  if (!(pt[PT_INDEX(addr)] & PAGE_PRESENT)) return 0;

  uint64_t page = pt[PT_INDEX(addr)] & ~0xFFFULL;
  return page | (addr & 0xFFF);
}

void arch_virt_map() {
  kprintf("virt: Setup paging...\n");
  kprintf("virt: Mapping physical memory to virtual\n");

  uint64_t pml4_phys = kernel_phys_alloc(1);
  uint64_t *pml4 = (uint64_t*) P2V(pml4_phys);
  memset(pml4, 0, PAGE_SIZE);

  for (uint64_t i = 0; i < kernel.mem.map.count; i++) {
    memreg_t *region = &kernel.mem.map.regions[i];

    if (region->type == MEMREG_TYPE_BAD_MEMORY || region->type == MEMREG_TYPE_RESERVED) {
      continue;
    }

    uint64_t base = region->base;
    uint64_t size = region->size;

    kprintf("virt: mapping %08p %08p %s\n", base, size, memreg_type_to_str(region->type));

    if (size % PAGE_SIZE != 0) size += PAGE_SIZE;

    for (uint64_t offset = 0; offset < size; offset += PAGE_SIZE) {
      uint64_t phys = base + offset;
      uint64_t virt_hhdm = kernel.mem.hhdm + phys;

      // Ensure HHDM is always Read/Write
      // Even if it's Kernel code, we might need to write to it via HHDM (e.g., for GDB stubs or patching)
      arch_virt_map_page(pml4, (void *)virt_hhdm, phys);
    }
  }

  // Map the Kernel
  uint64_t kernel_phys_base = kernel.mem.kernel_phys_base;
  uint64_t kernel_virt_base = kernel.mem.kernel_virt_base;

  extern uint64_t __kernel_start;
  extern uint64_t __kernel_end;

  uint64_t kernel_start_v = (uint64_t) &__kernel_start;
  uint64_t kernel_end_v   = (uint64_t) &__kernel_end;
  uint64_t kernel_size    = kernel_end_v - kernel_start_v;

  kprintf("virt: Mapping Kernel: virt %016p phys 0x%16p (size %d)\n", kernel_virt_base, kernel_phys_base, kernel_size);

  for (uint64_t offset = 0; offset < kernel_size; offset += PAGE_SIZE) {
    uint64_t curr_virt = kernel_start_v + offset;
    uint64_t curr_phys = kernel_phys_base + (curr_virt - kernel_virt_base);

    arch_virt_map_page(pml4, (void*)curr_virt, curr_phys);
  }

  kprintf("virt: Loading new CR3...\n");

  load_cr3(pml4_phys);

  kprintf("virt: Paging enabled successfully\n");

  kernel.arch.page_table = (void *) P2V(pml4_phys);
}
