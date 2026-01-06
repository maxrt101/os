#include <util/util.h>
#include <x86_64.h>
#include <kernel.h>

#define __GET_STUB(__i) ((void (**)())isr_stub_table)[__i]

#define IDT_MAX_INTERRUPTS 256

#define IDT_DESC_INT_BIT_16   0x06
#define IDT_DESC_TRAP_BIT_16  0x07
#define IDT_DESC_INT_BIT_32   0x0E
#define IDT_DESC_TRAP_BIT_32  0x0F
#define IDT_DESC_RING_0       0x00
#define IDT_DESC_RING_1       0x20
#define IDT_DESC_RING_2       0x40
#define IDT_DESC_RING_3       0x60
#define IDT_DESC_PRESENT      0x80

#define SCHED_TRIGGER_IRQ 32

typedef __PACKED_STRUCT {
  uint16_t offset_1;
  uint16_t segment_selector;
  uint8_t  ist;
  uint8_t  flags;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_1;
} idt_desc_t;

typedef __PACKED_STRUCT {
  uint16_t limit;
  uint64_t base;
} idtr_t;

extern void * isr_stub_table[];

void * x86_64_exception_handler(x86_64_irq_frame_t * frame) {
  if (frame->irq < 32) {
    kprintf("\nException: %s (error_code=0x%lx)\n", x86_64_get_exc_name(frame->irq), frame->err);
    x86_64_dump_irq_frame(frame);
    x86_64_stack_trace(frame->rbp, frame->rip);
    kprintf("Aborting now...\n");
    arch_abort();
  } else {
    x86_64_irq_done(frame->irq);

    x86_64_irq_handler_t * table = kernel.arch.irq;

    if (table[frame->irq]) {
      table[frame->irq](frame);
    } else {
      kprintf("Warning: IRQ #%lu was triggered, but has no handler\n", frame->irq);
    }
  }

  if (frame->irq == SCHED_TRIGGER_IRQ) {
    return sched_switch(&kernel.sched, frame);
  }

  return frame;
}

__STATIC_INLINE void idt_load(idtr_t * idtr) {
  ARCH_ASM("lidt (%0)" : : "r" (idtr));
}

__STATIC_INLINE void idt_register_irq_handler(idt_desc_t * idt, uint8_t vector, void * isr, uint8_t flags, uint8_t ist) {
  idt_desc_t *desc = &idt[vector];

  uint64_t addr = (uint64_t) isr;

  desc->offset_1 = addr & 0xFFFF;
  desc->offset_2 = (addr >> 16) & 0xFFFF;
  desc->offset_3 = (addr >> 32) & 0xFFFFFFFF;

  desc->segment_selector = KERNEL_CS;
  desc->ist              = ist;
  desc->flags            = flags;
  desc->reserved_1       = 0;
}

void x86_64_init_idt(kernel_t * kernel) {
  kernel->arch.idt = (void *) (kernel_phys_alloc(1) + kernel->mem.hhdm);

  idt_desc_t * idt = kernel->arch.idt;

  for (int i = 0; i < IDT_MAX_INTERRUPTS; i++) {
    idt_register_irq_handler(idt, i, isr_stub_table[i], IDT_DESC_PRESENT | IDT_DESC_INT_BIT_32 | IDT_DESC_RING_0, 0);
  }

  idt_register_irq_handler(idt, 0x08, __GET_STUB(0x08), IDT_DESC_PRESENT | IDT_DESC_INT_BIT_32 | IDT_DESC_RING_0, 1);
  idt_register_irq_handler(idt, 0x80, __GET_STUB(0x80), IDT_DESC_PRESENT | IDT_DESC_INT_BIT_32 | IDT_DESC_RING_3, 0);

  idtr_t idtr = {
    .limit = sizeof(idt_desc_t) * IDT_MAX_INTERRUPTS - 1,
    .base = (uint64_t) idt
  };

  idt_load(&idtr);

  kernel->arch.irq = (void *) (kernel_phys_alloc(1) + kernel->mem.hhdm);
}

void x86_64_irq_register_handler(uint32_t irq, void * handler) {
  x86_64_irq_handler_t * table = kernel.arch.irq;
  table[irq] = handler;
  kprintf("Register handler for IRQ #%d - %p\n", irq, handler);
}
