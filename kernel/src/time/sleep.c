#include <time/sleep.h>
#include <time/timeout.h>
#include <util/util.h>

#define VERBOSE_USLEEP 0

#if VERBOSE_USLEEP
#include "kernel.h"
#endif

void sleep(time_t duration) {
  time_t us = time_convert(duration, TIME_MICROSECONDS);
  usleep(us.value);
}

__OPTIMIZE(0) void usleep(uint64_t us) {
  timeout_t t;
  timeout_init(&t, time_microseconds(us));

  while (!timeout_is_expired(&t)) {
#if VERBOSE_USLEEP
    kprintf("%d/%d\r", timeout_elapsed(&t), t.ticks);
#endif
  }
#if VERBOSE_USLEEP
  kprintf("\n");
#endif
}