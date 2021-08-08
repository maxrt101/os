#include <sys/asm.h>

ASM_SECTION(".text32");

extern "C" void kboot_main() {
  int x = 0xfbcd;

  asm("cli\n");
  asm("hlt\n");
}