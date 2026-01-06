#include <x86_64.h>
#include <kernel.h>

#ifndef STACK_TRACE_DEPTH_LIMIT
#define STACK_TRACE_DEPTH_LIMIT 100
#endif

void x86_64_dump_irq_frame(x86_64_irq_frame_t * frame) {
  kprintf("R8     %016p | R9     %016p\n", frame->r8,  frame->r9);
  kprintf("R10    %016p | R11    %016p\n", frame->r10, frame->r11);
  kprintf("R12    %016p | R13    %016p\n", frame->r12, frame->r13);
  kprintf("R14    %016p | R15    %016p\n", frame->r14, frame->r15);

  kprintf("RBP    %016p | RDI    %016p\n", frame->rbp, frame->rdi);
  kprintf("RSI    %016p | RDX    %016p\n", frame->rsi, frame->rdx);
  kprintf("RCX    %016p | RBX    %016p\n", frame->rcx, frame->rbx);
  kprintf("RAX    %016p | RFLAGS %016p\n", frame->rax, frame->rflags);

  kprintf("RIP    %016p | RSP    %016p\n", frame->rip, frame->rsp);
  kprintf("CS     %016p | SS     %016p\n", frame->cs,  frame->ss);
}

void x86_64_stack_trace(uint64_t rbp, uint64_t rip) {
  kprintf("Stack trace:\n");
  kprintf("Frame #0 - %p\n", rip);

  x86_64_stack_frame_t * frame = (x86_64_stack_frame_t *) rbp;

  uint32_t f;
  for (f = 1; f <= STACK_TRACE_DEPTH_LIMIT && frame->rbp; ++f) {
    kprintf("Frame #%d - %p\n", f, frame->rip);
    frame = frame->rbp;
  }

  if (f == STACK_TRACE_DEPTH_LIMIT) {
    kprintf("...\n");
#if SHOW_TIPS
    kprintf("Stack trace limit reached, recompile with STACK_TRACE_DEPTH_LIMIT changed\n");
#endif
  }
}

const char * x86_64_get_exc_name(uint64_t exc) {
  switch (exc) {
    case 0:  return "Divide By Zero Error";
    case 1:  return "Debug";
    case 2:  return "NMI";
    case 3:  return "Breakpoint";
    case 4:  return "Overflow";
    case 5:  return "Bound Range Exceeded";
    case 6:  return "Invalid Opcode";
    case 7:  return "Device not available";
    case 8:  return "Double Fault";
    case 9:  return "x87 Segment Overrun";
    case 10: return "Invalid TSS";
    case 11: return "Segment Not Present";
    case 12: return "Stack-Segment Fault";
    case 13: return "General Protection Fault";
    case 14: return "Page Fault";
    case 16: return "x87 FPU error";
    case 17: return "Alignment Check";
    case 18: return "Machine Check";
    case 19: return "SIMD (SSE/AVX) error";
    default: return "Unknown";
  }
}

void arch_stacktrace(void) {
  uint64_t rbp, rip;
  ARCH_ASM(
    "mov %%rbp, %0  \n"
    "call next      \n" // A trick to get RIP: a call will push RIP onto the stack
    "next:          \n"
    "pop %%rax      \n"
    "mov %%rax, %1  \n"
    : "=m"(rbp), "=m"(rip)
    :
    : "rax"
  );
  x86_64_stack_trace(rbp, rip);
}
