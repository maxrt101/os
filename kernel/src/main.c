#include <drivers/keyboard/keyboard.h>
#include <monitor/monitor.h>
#include <event/event.h>
#include <util/util.h>
#include <kernel.h>
#include <arch.h>

#include "memory/virtmem.h"

#define FB_TEST 0

kernel_t kernel;

static event_ctx_t kernel_event_sys_timer;

__STATIC_INLINE void kinit_events(kernel_t * kernel) {
  event_register(KERNEL_EVENT_SYS_TIMER, &kernel_event_sys_timer);
}

void kinit(kernel_t * kernel) {
  arch_disable_interrupts();
  kinit_events(kernel);
  kinit_port(kernel);
  kinit_io(kernel);
  kprintf("Initializing kernel\n");
  memmap_dump(&kernel->memmap);
  kernel_phys_map(&kernel->memmap);
  arch_init(kernel);
  // kernel_virt_map();
  arch_enable_interrupts();
}

#if FB_TEST
void fb_test() {
  framebuffer_draw_line(&kernel.framebuffer, (position_t){100, 200}, (position_t){200, 300}, (color_t){255, 0, 0});
  framebuffer_draw_line(&kernel.framebuffer, (position_t){200, 300}, (position_t){300, 400}, (color_t){0, 255, 0});
  framebuffer_draw_line(&kernel.framebuffer, (position_t){300, 400}, (position_t){400, 500}, (color_t){0, 0, 255});
  framebuffer_scroll(&kernel.framebuffer, 100, (color_t){128, 128, 128});
}
#endif

int kmain(void) {
  kinit(&kernel);

  UTIL_IF_1(FB_TEST, fb_test());

  kprintf("Kernel initialized\n");
  kprintf("Kernel HHDM offset: %p\n", kernel.hhdm.offset);

  monitor_main();

  arch_abort();
  return 0;
}
