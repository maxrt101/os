#include <x86_64.h>

#include "kernel.h"

typedef void (*x86_64_irq_handler_t)(void *);

#define HAS_ERROR_CODE 1

#define IRQ_NAME(__n) \
  irq_ ## __n

#define IRQ_SIGNATURE_RAW(__n, ...) \
  __INTERRUPT void IRQ_NAME(__n)(void * frame, ## __VA_ARGS__)

#define IRQ_SIGNATURE(__n, ...)                                   \
  UTIL_IF_EMPTY(__VA_ARGS__,                                      \
    IRQ_SIGNATURE_RAW(__n),                                       \
    IRQ_SIGNATURE_RAW(__n, uint64_t code))

#define IRQ_EXC_BODY(__n, __err_code)                             \
  {                                                               \
    x86_64_irq_exc_handler_ctx_t payload = {                      \
      .cpu_frame = frame,                                         \
      .irq = __n,                                                 \
      .error_code = __err_code                                    \
    };                                                            \
    exception_handler(&payload);                                  \
  }

#define IRQ_EXC_DEF(__n, ...)                                     \
  IRQ_SIGNATURE(__n, __has_code)                                  \
  IRQ_EXC_BODY(__n, UTIL_IF_1(__VA_ARGS__, code, 0))

#define IRQ_DEF(__n)                                              \
  IRQ_SIGNATURE(__n) {                                            \
    x86_64_irq_handler_ctx_t payload = {                          \
      .cpu_frame = frame,                                         \
      .irq = __n,                                                 \
    };                                                            \
    irq_handler(&payload);                                        \
  }

#define IRQ_EXC_REGISTER(__idt, __n) \
  x86_64_idt_set_handler(__idt, __n, IRQ_TYPE_TRAP, (void *) IRQ_NAME(__n));

#define IRQ_REGISTER(__idt, __n, ...) \
  x86_64_idt_set_handler(__idt, __n, UTIL_IF_EMPTY(__VA_ARGS__, IRQ_TYPE_INTERRUPT, __VA_ARGS__), (void *) IRQ_NAME(__n));

#define GET_IRQ_FRAME(__name, __rsp, __cpu_frame_type) \
  x86_64_irq_frame_t * __name = (x86_64_irq_frame_t *) (__rsp - sizeof(x86_64_irq_frame_t) - sizeof(__cpu_frame_type));

static void exception_handler(x86_64_irq_exc_handler_ctx_t * ctx) {
  kprintf("\nException: %s (error_code=%d)\n", x86_64_get_exc_name(ctx->irq), ctx->error_code);

  GET_IRQ_FRAME(frame, ctx->cpu_frame->rsp, x86_64_irq_exc_frame_cpu_t);

  x86_64_dump_irq_exc_cpu_frame(ctx->cpu_frame);
  x86_64_dump_irq_frame(frame);
  x86_64_stack_trace(frame->rbp, frame->rip);

  kprintf("Aborting now...\n");
  arch_abort();
}

volatile uint64_t last_irq = 0;

static void irq_handler(x86_64_irq_handler_ctx_t * ctx) {
  x86_64_irq_handler_t * table = kernel.arch.irq;
  x86_64_irq_handler_t handler = table[ctx->irq];

  last_irq = ctx->irq;

  if (handler) {
    handler(ctx);
  } else {
    kprintf("Warning: IRQ #%d was triggered, but has no handler\n", ctx->irq);

    GET_IRQ_FRAME(frame, ctx->cpu_frame->rsp, x86_64_irq_frame_cpu_t);

    x86_64_dump_irq_cpu_frame(ctx->cpu_frame);
    x86_64_dump_irq_frame(frame);
    x86_64_stack_trace(frame->rbp, frame->rip);

    kprintf("Resuming execution...\n");
  }

  x86_64_irq_done(ctx->irq);
}

IRQ_EXC_DEF(0);
IRQ_EXC_DEF(1);
IRQ_EXC_DEF(2);
IRQ_EXC_DEF(3);
IRQ_EXC_DEF(4);
IRQ_EXC_DEF(5);
IRQ_EXC_DEF(6);
IRQ_EXC_DEF(7);
IRQ_EXC_DEF(8,  HAS_ERROR_CODE);
IRQ_EXC_DEF(9);
IRQ_EXC_DEF(10, HAS_ERROR_CODE);
IRQ_EXC_DEF(11, HAS_ERROR_CODE);
IRQ_EXC_DEF(12, HAS_ERROR_CODE);
IRQ_EXC_DEF(13, HAS_ERROR_CODE);
IRQ_EXC_DEF(14, HAS_ERROR_CODE);
IRQ_EXC_DEF(15);
IRQ_EXC_DEF(16);
IRQ_EXC_DEF(17, HAS_ERROR_CODE);
IRQ_EXC_DEF(18);
IRQ_EXC_DEF(19);

IRQ_DEF(32);
IRQ_DEF(33);
IRQ_DEF(34);
IRQ_DEF(35);
IRQ_DEF(36);
IRQ_DEF(37);
IRQ_DEF(38);
IRQ_DEF(39);
IRQ_DEF(40);
IRQ_DEF(41);
IRQ_DEF(42);
IRQ_DEF(43);
IRQ_DEF(44);
IRQ_DEF(45);
IRQ_DEF(46);
IRQ_DEF(47);
IRQ_DEF(48);

void x86_64_irq_register_handler(uint32_t irq, void * handler) {
  x86_64_irq_handler_t * table = kernel.arch.irq;
  table[irq] = handler;
  kprintf("Register handler for IRQ #%d - %p\n", irq, handler);
}

void x86_64_init_irq(kernel_t * kernel, void * idt, size_t irq_count) {
  kernel->arch.irq = (void *) (kpalloc(1) + kernel->hhdm.offset);

  IRQ_EXC_REGISTER(idt, 0);
  IRQ_EXC_REGISTER(idt, 1);
  IRQ_EXC_REGISTER(idt, 2);
  IRQ_EXC_REGISTER(idt, 3);
  IRQ_EXC_REGISTER(idt, 4);
  IRQ_EXC_REGISTER(idt, 5);
  IRQ_EXC_REGISTER(idt, 6);
  IRQ_EXC_REGISTER(idt, 7);
  IRQ_EXC_REGISTER(idt, 8);
  IRQ_EXC_REGISTER(idt, 9);
  IRQ_EXC_REGISTER(idt, 10);
  IRQ_EXC_REGISTER(idt, 11);
  IRQ_EXC_REGISTER(idt, 12);
  IRQ_EXC_REGISTER(idt, 13);
  IRQ_EXC_REGISTER(idt, 14);
  IRQ_EXC_REGISTER(idt, 15);
  IRQ_EXC_REGISTER(idt, 16);
  IRQ_EXC_REGISTER(idt, 17);
  IRQ_EXC_REGISTER(idt, 18);
  IRQ_EXC_REGISTER(idt, 19);

  IRQ_REGISTER(idt, 32);
  IRQ_REGISTER(idt, 33);
  IRQ_REGISTER(idt, 34);
  IRQ_REGISTER(idt, 35);
  IRQ_REGISTER(idt, 36);
  IRQ_REGISTER(idt, 37);
  IRQ_REGISTER(idt, 38);
  IRQ_REGISTER(idt, 39);
  IRQ_REGISTER(idt, 40);
  IRQ_REGISTER(idt, 41);
  IRQ_REGISTER(idt, 42);
  IRQ_REGISTER(idt, 43);
  IRQ_REGISTER(idt, 44);
  IRQ_REGISTER(idt, 45);
  IRQ_REGISTER(idt, 46);
  IRQ_REGISTER(idt, 47);
  IRQ_REGISTER(idt, 48);

  // TODO: Define the rest of the IRQs as software

  kprintf("Initialized IRQs\n");
}
