section .entry
bits 16

extern main

jmp main

; entry:
;   mov ebp, kernel_stack_top       ; Set new stack base
;   mov esp, ebp

;   ; mov  eax, 0x2BADB002              ; multiboot specs say eax should be this

;   extern main
;   call main

;   cli
;   hlt

; section .bss
; align 4
; kernel_stack_bottom: equ $
;   resb 16384 ; 16 KB
; kernel_stack_top: