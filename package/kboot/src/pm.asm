bits 16
section .text16

extern kboot_main
global switch_to_pm

CODE_SEG equ 8
DATA_SEG equ 16

switch_to_pm:
  cli
  hlt
  mov eax, cr0
  or  eax, 0x1
  mov cr0, eax

  mov ax, DATA_SEG                ; Update segment registers
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  jmp kboot_main

  cli
  hlt