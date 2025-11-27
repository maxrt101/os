#pragma once

#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(addr) (((addr) / PAGE_SIZE) * PAGE_SIZE)
#define PAGE_ALIGN_UP(addr)   ((((addr) + PAGE_SIZE-1) / PAGE_SIZE) * PAGE_SIZE)
