#pragma once

#include <util/util.h>
#include <stdint.h>
#include <stddef.h>

#define PIT_COUNTER_0           0
#define PIT_COUNTER_1           0x40
#define PIT_COUNTER_2           0x80

#define PIT_MODE_TERMINALCOUNT  0
#define PIT_MODE_ONESHOT        0x2
#define PIT_MODE_RATEGEN        0x4
#define PIT_MODE_SQUAREWAVEGEN  0x6
#define PIT_MODE_SOFTWARETRIG   0x8
#define PIT_MODE_HARDWARETRIG   0xA

#define PIT_FREQ_DEFAULT        100 // Hz

#define PIC0_BASE               0x20
#define PIC1_BASE               0x28

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_DS   0x18
#define USER_CS   0x20
#define TSS_SEG   0x28

typedef enum {
  IRQ_TYPE_INTERRUPT  = 0x8E,
  IRQ_TYPE_TRAP       = 0x8F,
  IRQ_TYPE_SOFTWARE   = 0xEF
} irq_type_t;

typedef __PACKED_STRUCT x86_64_stack_frame_t {
  struct x86_64_stack_frame_t * rbp;
  uint64_t rip;
} x86_64_stack_frame_t;

typedef __PACKED_STRUCT {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

  uint64_t irq;
  uint64_t err;

  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;       // Note: Only pushed if privilege change (Ring 3->0) occurred
  uint64_t ss;        // Note: Only pushed if privilege change occurred
} x86_64_irq_frame_t;

typedef void (*x86_64_irq_handler_t)(x86_64_irq_frame_t *);

typedef __PACKED_STRUCT {
  uint32_t reserved_0;
  uint64_t rsp0;       // Stack pointer for Ring 0 (Kernel) interrupts
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved_1;
  uint64_t ist[7];     // Interrupt Stack Table (IST) 1-7
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iomap_base;
} x86_64_tss_t;

typedef void * arch_task_ctx_t;

typedef struct arch_ctx_t {
  void * tss;
  void * gdt;
  void * idt;
  void * irq;
  void * page_table;
} arch_ctx_t;

typedef struct kernel_t kernel_t;

#define ARCH_ASM_GLOBAL(...) asm (__VA_ARGS__)
#define ARCH_ASM(...) asm volatile (__VA_ARGS__)

#define ARCH_BKPT() ARCH_ASM("int $3")

__STATIC_INLINE uint8_t inb(uint16_t port) {
  uint8_t ret;
  ARCH_ASM("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

__STATIC_INLINE void outb(uint16_t port, uint8_t val) {
  ARCH_ASM("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void x86_64_init_tss(kernel_t * kernel);
void x86_64_init_gdt(kernel_t * kernel);
void x86_64_init_idt(kernel_t * kernel);
void x86_64_init_pic(uint8_t base0, uint8_t base1);
void x86_64_init_pit(uint32_t freq, uint8_t counter, uint8_t mode);
void x86_64_init_ps2();

void x86_64_irq_register_handler(uint32_t irq, void * handler);

void x86_64_irq_done(uint64_t n);
void x86_64_irq_unmask(uint64_t n);
void x86_64_irq_mask(uint64_t n);

const char * x86_64_get_exc_name(uint64_t exc);

void x86_64_dump_irq_frame(x86_64_irq_frame_t * frame);
void x86_64_stack_trace(uint64_t rbp, uint64_t rip);
