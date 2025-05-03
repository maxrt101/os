#include <util/util.h>
#include <x86_64.h>
#include <kernel.h>

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#define USE_TSS 0

#if USE_TSS
#define GDT_MAX_DESCRIPTORS 5
#else
#define GDT_MAX_DESCRIPTORS 3
#endif

#define GDT_DESC_ACCESS         0x01 /** Access bit */
#define GDT_DESC_RW             0x02 /** Descriptor is readable and writable. Default - read only */
#define GDT_DESC_EXP            0x04 /** Expansion direction bit */
#define GDT_DESC_EXEC_CODE      0x08 /** Executable code segment. Default - data segment */
#define GDT_DESC_CODE_DATA      0x10 /** Code or data segment. Default - system defined descriptor */
#define GDT_DESC_DPL            0x60 /** DPL bits */
#define GDT_DESC_MEMORY         0x80 /** "In memory" bit */

#define GDT_FLAG_LONG           0b0010

#define GDT_GRAND_32BIT         0x40  /** Set if 32 bits. Default - 16 bits */
#define GDT_GRAND_4K            0x80  /** 4K granularity. Default - none */

#define LDT_DESC_LDT            0x2
#define LDT_DESC_AVAILABLE_TSS  0x9
#define LDT_DESC_BUSY_TSS       0xB

#define LDT_DESC_PRESENT        0x80

typedef __PACKED_STRUCT {
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t  base_2;
  uint8_t  access;
  __PACKED_STRUCT {
    uint8_t limit_2 : 4;
    uint8_t flags   : 4;
  };
  uint8_t  base_3;
} gdt_desc_t;

typedef __PACKED_STRUCT {
  uint16_t limit;
  uint64_t base;
} gdtr_t;

typedef __PACKED_STRUCT {
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t  base_2;
  uint8_t  access;
  __PACKED_STRUCT {
    uint8_t limit_2 : 4;
    uint8_t flags   : 4;
  };
  uint8_t  base_3;
  uint32_t base_4;
  uint32_t reserved_0;
} ldt_desc_t;

__STATIC_INLINE void gdt_load(gdtr_t * gdtr) {
  ARCH_ASM("lgdt (%0)" : : "r" (gdtr));

#if USE_TSS
  ARCH_ASM("mov $0x18, %%ax\n"
           "ltr %%ax"
           ::: "eax");
#endif
}

__STATIC_INLINE void reload_segment_regs() {
  ARCH_ASM(
    "mov %[ds], %%ax           \n"
    "mov %%ax, %%ds            \n"
    "mov %%ax, %%es            \n"
    "mov %%ax, %%ss            \n"
    "mov %%ax, %%fs            \n"
    "mov %%ax, %%gs            \n"
    "pushq %[cs]               \n"
    "lea 1f(%%rip), %%rax      \n"
    "pushq %%rax               \n"
    "lretq                     \n"
    "1:                        \n"
    :
    : [ds] "i"(KERNEL_DS),
      [cs] "i"(KERNEL_CS)
    : "rax", "memory"
  );
}

__STATIC_INLINE void gdt_init_descriptor(gdt_desc_t * gdt, uint32_t index, uint64_t base, uint64_t limit, uint8_t access, uint8_t flags) {
  gdt_desc_t * desc = &gdt[index];

  desc->base_1   = base & 0xFFFF;
  desc->base_2   = (base >> 16) & 0xFF;
  desc->base_3   = (base >> 24) & 0xFF;
  desc->limit_1  = limit & 0xFFFF;
  desc->limit_2  = (limit >> 16) & 0xF;
  desc->access   = access;
  desc->flags    = flags & 0xF;
}

__STATIC_INLINE void ldt_init_descriptor(gdt_desc_t * gdt, uint32_t index, uint64_t base, uint64_t limit, uint8_t access, uint8_t flags) {
  ldt_desc_t * desc =  (ldt_desc_t *) &gdt[index];

  desc->base_1   = base & 0xFFFF;
  desc->base_2   = (base >> 16) & 0xFF;
  desc->base_3   = (base >> 24) & 0xFF;
  desc->base_4   = (base >> 32) & 0xFFFFFFFF;
  desc->limit_1  = limit & 0xFFFF;
  desc->limit_2  = (limit >> 16) & 0xF;
  desc->access   = access;
  desc->flags    = flags & 0xF;
}

void x86_64_init_gdt(kernel_t * kernel) {
  kernel->arch.gdt = (void *) (kpalloc(1) + kernel->hhdm.offset);

  gdt_desc_t * gdt = kernel->arch.gdt;

  gdtr_t gdtr = {
    .limit = (sizeof(gdt_desc_t) * GDT_MAX_DESCRIPTORS) - 1,
    .base = (uint64_t) gdt
  };

  gdt_init_descriptor(gdt, 0, 0, 0, 0, 0);

  gdt_init_descriptor(gdt, 1, 0, 0,
    GDT_DESC_RW | GDT_DESC_EXEC_CODE | GDT_DESC_CODE_DATA | GDT_DESC_MEMORY,
    GDT_FLAG_LONG
  );

  gdt_init_descriptor(gdt, 2, 0, 0,
    GDT_DESC_RW | GDT_DESC_CODE_DATA | GDT_DESC_MEMORY,
    0
  );

#if USE_TSS
  ldt_init_descriptor(gdt, 3, (uint64_t) kernel->arch.tss, sizeof(x86_64_tss_t) - 1,
     LDT_DESC_PRESENT | LDT_DESC_AVAILABLE_TSS,
  0
  );
#endif

  gdt_load(&gdtr);
  reload_segment_regs();
}

