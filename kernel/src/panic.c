#include <kernel.h>

void kpanic(const char * fmt, ...) {
  kprintf("\033[41mPANIC\033[0m: ");
  va_list args;
  va_start(args, fmt);
  vkprintf(fmt, args);
  va_end(args);
  arch_abort();
}
