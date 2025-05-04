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
  arch_ctx_t    arch;

  framebuffer_t framebuffer;
  tty_t         tty;

  memmap_t      memmap;
  kpalloc_t     kpalloc;

  struct {
    uint64_t    offset;
  } hhdm;
} kernel_t;

extern kernel_t kernel;

void kinit(kernel_t * kernel);
void kinit_port(kernel_t * kernel);
void kinit_io(kernel_t * kernel);

void vkprintf(const char * fmt, va_list args);
void kprintf(const char * fmt, ...);

char kgetch();
void kgetline(char * buf, size_t buf_size);
bool kgetline_async(char * buf, size_t buf_size, size_t * index);

void kpanic(const char * fmt, ...);
