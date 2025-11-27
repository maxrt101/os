#include <util/util.h>
#include <x86_64.h>
#include <kernel.h>

#define IDT_MAX_INTERRUPTS 250

#define IDT_DESC_INT_BIT_16   0x06
#define IDT_DESC_TRAP_BIT_16  0x07
#define IDT_DESC_INT_BIT_32   0x0E
#define IDT_DESC_TRAP_BIT_32  0x0F
#define IDT_DESC_RING_1       0x40
#define IDT_DESC_RING_2       0x20
#define IDT_DESC_RING_3       0x60
#define IDT_DESC_PRESENT      0x80

#define IDT_SEG_SEL_DEFAULT 0x8
#define IDT_FLAGS_DEFAULT   IRQ_TYPE_INTERRUPT
// #define IDT_SEG_SEL_DEFAULT 0x18
// #define IDT_FLAGS_DEFAULT   (IDT_DESC_PRESENT | IDT_DESC_INT_BIT_32)

typedef __PACKED_STRUCT {
  uint16_t offset_1;
  uint16_t segment_selector;
  // __PACKED_STRUCT {
  //   uint8_t ist : 3;
  //   uint8_t reserved_0 : 5;
  // };
  uint8_t ist;
  uint8_t  flags;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_1;
} idt_desc_t;

typedef __PACKED_STRUCT {
  uint16_t limit;
  uint64_t base;
} idtr_t;

__STATIC_INLINE void idt_load(idtr_t * idtr) {
  ARCH_ASM("lidt (%0)" : : "r" (idtr));
}

__STATIC_INLINE void idt_register_irq_handler(idt_desc_t * idt, uint64_t isr, uint16_t flags, uint16_t sel, void * handler) {
  idt_desc_t * desc = &idt[isr];
  uint64_t handler_base = (uint64_t) handler;

  desc->offset_1         =  handler_base & 0xFFFF;
  desc->offset_2         = (handler_base >> 16) & 0xFFFF;
  desc->offset_3         = (handler_base >> 32) & 0xFFFFFFFF;
  desc->ist              = 0;
  // desc->reserved_0       = 0;
  desc->flags            = flags;
  desc->segment_selector = sel;
  desc->reserved_1       = 0;
}

void x86_64_init_idt(kernel_t * kernel) {
  kernel->arch.idt = (void *) (kernel_phys_alloc(1) + kernel->mem.hhdm);

  idt_desc_t * idt = kernel->arch.idt;

  idtr_t idtr = {
    .limit = sizeof(idt_desc_t) * IDT_MAX_INTERRUPTS - 1,
    .base = (uint64_t) idt
  };

  x86_64_init_irq(kernel, idt, IDT_MAX_INTERRUPTS);

  idt_load(&idtr);
}

void x86_64_idt_set_handler(void * idt, uint64_t irq, irq_type_t type, void * handler) {
  idt_register_irq_handler(idt, irq, type, IDT_SEG_SEL_DEFAULT, handler);
}
