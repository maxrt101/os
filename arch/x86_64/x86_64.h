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

typedef enum {
  IRQ_TYPE_INTERRUPT  = 0x8E,
  IRQ_TYPE_TRAP       = 0x8F,
  IRQ_TYPE_SOFTWARE   = 0xEF
} irq_type_t;

typedef __PACKED_STRUCT x86_64_stack_frame_t {
  struct x86_64_stack_frame_t * rbp;
  uint64_t rip;
} x86_64_stack_frame_t;

// TODO: Make one x86_64_irq_frame_t, just calculate offsets differently in exc_handler and irq_handler

typedef __PACKED_STRUCT {
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} x86_64_irq_frame_cpu_t;

typedef __PACKED_STRUCT {
  uint64_t rax;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t rbp;
  x86_64_irq_frame_cpu_t cpu;
} x86_64_irq_frame_t;

typedef struct {
  x86_64_irq_frame_cpu_t * cpu_frame;
  uint64_t irq;
} x86_64_irq_handler_ctx_t;

typedef __PACKED_STRUCT {
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} x86_64_irq_exc_frame_cpu_t;

typedef __PACKED_STRUCT {
  uint64_t rax;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t rbp;
  uint64_t rip;
  x86_64_irq_exc_frame_cpu_t cpu;
} x86_64_irq_exc_frame_t;

typedef struct {
  x86_64_irq_exc_frame_cpu_t * cpu_frame;
  uint64_t error_code;
  uint64_t irq;
} x86_64_irq_exc_handler_ctx_t;

typedef __PACKED_STRUCT {
  uint32_t reserved_0;
  uint64_t rsp0;
  uint32_t reserved_1[23];
} x86_64_tss_t;

typedef struct arch_ctx_t {
  void * tss;
  void * gdt;
  void * idt;
  void * irq;
} arch_ctx_t;

typedef struct kernel_t kernel_t;

#define ARCH_ASM_GLOBAL(...) asm (__VA_ARGS__)
#define ARCH_ASM(...) asm volatile (__VA_ARGS__)

#define ARCH_BKPT() ARCH_ASM("int 3")

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
void x86_64_init_irq(kernel_t * kernel, void * idt, size_t irq_count);
void x86_64_init_ps2();

void x86_64_irq_register_handler(uint32_t irq, void * handler);

void x86_64_irq_unmask(uint64_t n);
void x86_64_irq_mask(uint64_t n);

void x86_64_idt_set_handler(void *idt, uint64_t irq, irq_type_t type, void * handler);
void x86_64_irq_done(uint64_t n);

const char * x86_64_get_exc_name(uint64_t exc);

void x86_64_dump_irq_frame(x86_64_irq_frame_t * frame);
void x86_64_dump_irq_exc_frame(x86_64_irq_exc_frame_t * frame);
void x86_64_stack_trace(uint64_t rbp, uint64_t rip);
