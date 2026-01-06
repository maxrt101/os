#pragma once

#include <drivers/video/framebuffer.h>
#include <drivers/tty/tty.h>
#include <memory/memmap.h>
#include <memory/physmem.h>
#include <arch.h>
#include <stdarg.h>

enum kernel_events_e {
  KERNEL_EVENT_RESERVED = 0,
  KERNEL_EVENT_SYS_TIMER
};

typedef struct kernel_t {
  arch_ctx_t            arch;

  framebuffer_t         framebuffer;
  tty_t                 tty;

  struct {
    memmap_t            map;
    kernel_phys_alloc_t phys;

    uint64_t            hhdm;

    uint64_t            kernel_phys_base;
    uint64_t            kernel_virt_base;
  } mem;
} kernel_t;

extern kernel_t kernel;

void kernel_init(kernel_t * kernel);
void kernel_init_port(kernel_t * kernel);
void kernel_init_io(kernel_t * kernel);

void vkprintf(const char * fmt, va_list args);
void kprintf(const char * fmt, ...)
  __attribute__((__format__ (__printf__, 1, 2)));

char kgetch();
void kgetline(char * buf, size_t buf_size);
bool kgetline_async(char * buf, size_t buf_size, size_t * index);

void kpanic(const char * fmt, ...);
