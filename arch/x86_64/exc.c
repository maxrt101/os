#include <x86_64.h>
#include <kernel.h>

#ifndef STACK_TRACE_DEPTH_LIMIT
#define STACK_TRACE_DEPTH_LIMIT 100
#endif

void x86_64_dump_irq_frame(x86_64_irq_frame_t * frame) {
  kprintf("RIP    %016p | RSP    %016p\n", frame->cpu.rip, frame->cpu.rsp);
  kprintf("CS     %016p | SS     %016p\n", frame->cpu.cs,  frame->cpu.ss);
  kprintf("RBP    %016p | RFLAGS %016p\n", frame->rbp,     frame->cpu.rflags);

  kprintf("R8     %016p | R9     %016p\n", frame->r8,  frame->r9);
  kprintf("R10    %016p | R11    %016p\n", frame->r10, frame->r11);
  kprintf("RDI    %016p | RSI    %016p\n", frame->rdi, frame->rsi);
  kprintf("RAX    %016p | RCX    %016p\n", frame->rax, frame->rcx);
  kprintf("RDX    %016p\n", frame->rdx);
}

void x86_64_dump_irq_exc_frame(x86_64_irq_exc_frame_t * frame) {
  kprintf("RSP    %016p | RFLAGS %016p\n", frame->cpu.rsp, frame->cpu.rflags);
  kprintf("CS     %016p | SS     %016p\n", frame->cpu.cs,  frame->cpu.ss);

  kprintf("R8     %016p | R9     %016p\n", frame->r8,  frame->r9);
  kprintf("R10    %016p | R11    %016p\n", frame->r10, frame->r11);
  kprintf("RIP    %016p | RBP    %016p\n", frame->rip, frame->rbp);
  kprintf("RDI    %016p | RSI    %016p\n", frame->rdi, frame->rsi);
  kprintf("RAX    %016p | RCX    %016p\n", frame->rax, frame->rcx);
  kprintf("RDX    %016p\n", frame->rdx);
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
