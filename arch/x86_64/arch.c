#include <arch.h>
#include <x86_64.h>
#include <kernel.h>

void arch_init(kernel_t * kernel) {
  x86_64_init_tss(kernel);
  x86_64_init_gdt(kernel);
  x86_64_init_idt(kernel);
  x86_64_init_pic(PIC0_BASE, PIC1_BASE);
  x86_64_init_pit(PIT_FREQ_DEFAULT, PIT_COUNTER_0, PIT_MODE_SQUAREWAVEGEN);
  x86_64_init_ps2();
}

void arch_abort(void) {
  ARCH_ASM("cli");
  while (1) {
    ARCH_ASM("hlt");
  }
}

uint64_t arch_get_tick_duration_hz(void) {
  return PIT_FREQ_DEFAULT;
}

void arch_trigger_irq(uint32_t n) {
#if USE_WAIT_BEFORE_TRIGGERING_IRQ
  for (size_t i = 0; i < 500000000; ++i) {
    ;
  }
#endif

  switch (n) {
    case 32: ARCH_ASM("int $32"); break;
    case 33: ARCH_ASM("int $33"); break;
    case 34: ARCH_ASM("int $34"); break;
    case 35: ARCH_ASM("int $35"); break;
    case 36: ARCH_ASM("int $36"); break;
    case 37: ARCH_ASM("int $37"); break;
    case 38: ARCH_ASM("int $38"); break;
    case 39: ARCH_ASM("int $39"); break;
    case 40: ARCH_ASM("int $40"); break;
    case 41: ARCH_ASM("int $41"); break;
    case 42: ARCH_ASM("int $42"); break;
    case 43: ARCH_ASM("int $43"); break;
    case 44: ARCH_ASM("int $44"); break;
    case 45: ARCH_ASM("int $45"); break;
    case 46: ARCH_ASM("int $46"); break;
    case 47: ARCH_ASM("int $47"); break;
    case 48: ARCH_ASM("int $48"); break;
    default:
#if SHOW_TIPS
      kprintf("Unsupported IRQ for arch_trigger_irq (%d)\n", n);
#endif
      break;
  }
}

void arch_enable_interrupts(void) {
  ARCH_ASM("sti");
}

void arch_disable_interrupts(void) {
  ARCH_ASM("cli");
}
