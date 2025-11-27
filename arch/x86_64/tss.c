#include <x86_64.h>
#include <memory/physmem.h>
#include <kernel.h>

void x86_64_init_tss(kernel_t * kernel) {
  kernel->arch.tss = (void *) (kernel_phys_alloc(1) + kernel->mem.hhdm);
  x86_64_tss_t * tss = kernel->arch.tss;
  tss->rsp0 = 0xFFFFFFFFFFFFF000LL; // kernel stack
}
