#pragma once

#include <stddef.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define PAGE_ALIGN_DOWN(addr) (((addr) / PAGE_SIZE) * PAGE_SIZE)
#define PAGE_ALIGN_UP(addr)   ((((addr) + PAGE_SIZE-1) / PAGE_SIZE) * PAGE_SIZE)

void kheap_init(void);
void * kmalloc(size_t size);
void kfree(void * ptr);
