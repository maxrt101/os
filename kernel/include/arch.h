#pragma once

#include <stdint.h>

#ifndef ARCH
#error "No ARCH defined"
#endif

#ifndef ARCH_INC
#error "No ARCH_INC defined"
#else
#include ARCH_INC
#endif

typedef struct kernel_t kernel_t;

void arch_init(kernel_t * kernel);
void arch_abort(void);
void arch_stacktrace(void);

uint64_t arch_get_tick(void);
uint64_t arch_get_tick_duration_hz(void);

void arch_trigger_irq(uint32_t n);

void arch_enable_interrupts(void);
void arch_disable_interrupts(void);

void arch_virt_map(void);
// void arch_virt_map_page(void * vaddr, uint64_t paddr, uint64_t flags);
// void arch_virt_unmap_page(void * vaddr);
// uint64_t arch_virt_translate(void * vaddr);
