#include <x86_64.h>
#include <util/string.h>

arch_task_ctx_t arch_create_task_ctx(void * entry, void * arg, void * stack_top) {
  x86_64_irq_frame_t * frame = stack_top - sizeof(x86_64_irq_frame_t);

  memset(frame, 0, sizeof(x86_64_irq_frame_t));

  frame->rip    = (uint64_t) entry;
  frame->cs     = KERNEL_CS;
  frame->rflags = 0x202; // Interrupts Enabled (IF) | Reserved bit (1)
  frame->rsp    = (uint64_t) frame;
  frame->ss     = KERNEL_DS;

  frame->rdi    = (uint64_t) arg;

  frame->rbp    = (uint64_t) stack_top;

  return (void *) frame;
}
