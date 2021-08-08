#include "code16gcc.h"
#include "boot.h"

#include <sys/asm.h>

ASM_SECTION(".text16");

void fast_a20() {
  int data = inb(0x92);
  data |= 2;
  outb(0x92, data);
}

void enable_a20() {
  fast_a20();
}
