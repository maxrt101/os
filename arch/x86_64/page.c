#include <stdint.h>
#include <stddef.h>
#include <util/string.h>
#include <kernel.h>
#include <x86_64.h>
#include <util/util.h>
#include "memory/physmem.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 512
// #define KERNEL_OFFSET 0xFFFFFF8000000000
//                    0xffffffff80000000
#define KERNEL_PHYS_BASE 0xffffffff80000000
#define KERNEL_VIRT_BASE 0xFFFFFFFF80000000
// #define KERNEL_VIRT_BASE 0xFFFFFF8000000000
#define PAGE_PRESENT 0x001
#define PAGE_RW      0x002
#define PAGE_USER    0x004

#define PML4_INDEX(addr) (((uint64_t)(addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((uint64_t)(addr) >> 30) & 0x1FF)
#define PD_INDEX(addr)   (((uint64_t)(addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)   (((uint64_t)(addr) >> 12) & 0x1FF)

#define P2V(paddr) ((void *)((uintptr_t)(paddr) + kernel.hhdm.offset))
#define V2P(vaddr) ((uintptr_t)(vaddr) - kernel.hhdm.offset)

static void *alloc_page(void) {
    uint64_t phys = kernel_phys_alloc(1);
    void *virt = P2V(phys);
    memset(virt, 0, PAGE_SIZE);
    return virt;
}

__attribute__((always_inline))
static inline void load_cr3(uint64_t phys) {
    ARCH_ASM("mov %0, %%cr3" :: "r"(phys) : "memory");
}

static inline uint64_t read_cr3(void) {
    uint64_t value;
    ARCH_ASM("mov %%cr3, %0" : "=r"(value));
    return value;
}
void arch_virt_map_page(void *vaddr, uint64_t paddr, uint64_t flags) {
    uint16_t pml4_idx = PML4_INDEX(vaddr);
    uint16_t pdpt_idx = PDPT_INDEX(vaddr);
    uint16_t pd_idx   = PD_INDEX(vaddr);
    uint16_t pt_idx   = PT_INDEX(vaddr);

    uint64_t *pml4 = P2V(read_cr3());

    // Allocate tables as needed
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t pdpt_phys = kernel_phys_alloc(1);
        uint64_t *pdpt = P2V(pdpt_phys + kernel.hhdm.offset);
        memset(pdpt, 0, PAGE_SIZE);
        pml4[pml4_idx] = pdpt_phys | flags | PAGE_PRESENT;
    }

    uint64_t *pdpt = P2V(pml4[pml4_idx] & ~0xFFFUL);

    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t pd_phys = kernel_phys_alloc(1);
        uint64_t *pd = P2V(pd_phys + kernel.hhdm.offset);
        memset(pd, 0, PAGE_SIZE);
        pdpt[pdpt_idx] = pd_phys | flags | PAGE_PRESENT;
    }

    uint64_t *pd = P2V(pdpt[pdpt_idx] & ~0xFFFUL);

    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t pt_phys = kernel_phys_alloc(1);
        uint64_t *pt = P2V(pt_phys + kernel.hhdm.offset);
        memset(pt, 0, PAGE_SIZE);
        pd[pd_idx] = pt_phys | flags | PAGE_PRESENT;
    }

    uint64_t *pt = P2V(pd[pd_idx] & ~0xFFFUL);

    pt[pt_idx] = (paddr & ~0xFFFUL) | flags | PAGE_PRESENT;

    // Invalidate TLB entry for vaddr
    ARCH_ASM("invlpg (%0)" :: "r"(vaddr) : "memory");
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

    // ARCH_ASM("invlpg (%0)" :: "r"(vaddr) : "memory");
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

void arch_virt_map(void) {
    kprintf("Setup paging\n");

    // Allocate root page tables
    uint64_t *pml4 = (uint64_t *)(kernel_phys_alloc(1) + kernel.hhdm.offset);
    memset(pml4, 0, PAGE_SIZE);

    // Self-map page tables
    arch_virt_map_page(pml4, (uint64_t)pml4 - kernel.hhdm.offset, PAGE_PRESENT | PAGE_RW);

    // Map all usable memory regions:
    for (uint64_t i = 0; i < kernel.memmap.count; i++) {
        memreg_t *region = &kernel.memmap.regions[i];

        if (region->type != MEMREG_TYPE_USABLE)
            continue;

        uint64_t base = region->base;
        uint64_t size = region->size;

        for (uint64_t offset = 0; offset < size; offset += PAGE_SIZE) {
            uint64_t phys = base + offset;
            uint64_t virt_identity = phys;
            uint64_t virt_hhdm = kernel.hhdm.offset + phys;

            arch_virt_map_page((void *)virt_identity, phys, PAGE_PRESENT | PAGE_RW);
            arch_virt_map_page((void *)virt_hhdm,     phys, PAGE_PRESENT | PAGE_RW);
        }
    }

    // Map kernel binary to high half
    extern uint64_t _kernel_start;
    extern uint64_t _kernel_end;

    uint64_t kernel_start_phys = (uint64_t)&_kernel_start;
    uint64_t kernel_end_phys   = (uint64_t)&_kernel_end;
    uint64_t kernel_size       = kernel_end_phys - kernel_start_phys;

    for (uint64_t offset = 0; offset < kernel_size; offset += PAGE_SIZE) {
        uint64_t phys = KERNEL_PHYS_BASE + offset;
        uint64_t virt = KERNEL_VIRT_BASE + offset;

        arch_virt_map_page((void *)virt, phys, PAGE_PRESENT | PAGE_RW);
    }

    kprintf("Setting new page table\n");

    // Install new page tables
    load_cr3((uint64_t)pml4 - kernel.hhdm.offset);
}

