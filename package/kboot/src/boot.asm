;*******************************************************;
; boot.asm v0.1                                         ;
; by maxrt101                                           ;
;*******************************************************;

section .boot
bits 16
global boot

jmp boot

%define SECTORS      127
%define DRIVEADDR    0x1000

boot:
  mov [drive], dl                 ; Save drive num

  mov ax, 0x3
  int 0x10                        ; Set vga text mode 80x25

  mov ah, 0x2                     ; Read sectors
  mov ch, 0                       ; Cylinder idx
  mov dh, 0                       ; Head idx
  mov dl, [drive]                 ; Drive idx
  mov al, SECTORS                 ; Sectors to read

  mov cl, 2                       ; Sector idx
  mov bx, SECTOR2                 ; Target pointer

  int 0x13                        ; Sector-based hard drive and floppy disk read and write services

  xor eax, eax
  xor ebx, ebx

  mov ebx, dword [0x881c]         ; Save offset of data from kfs_node:1 into ebx

  mov edx, 0                      ; clear dividend
  mov eax, ebx                    ; divided
  mov ecx, 512                    ; divisor
  div ecx                         ; EAX = result, EDX = remainder

  add eax, 1
  mov word [sector_offset], ax

  mov ebx, dword [0x8808]         ; address now should be in ebx

  mov edx, 0                      ; clear dividend
  mov eax, ebx                    ; divided
  mov ecx, 512                    ; divisor
  div ecx                         ; EAX = result, EDX = remainder

  cmp edx, 0
  jz save_sector_cnt
  add eax, 1                      ; Add 1 to sector count, if there is a remainder

save_sector_cnt:
  mov word [sector_cnt], ax

  mov al, [sector_cnt]            ; Sectors to read

  mov ah, 0x2                     ; Read sectors
  mov ch, 0                       ; Cylinder idx
  mov dh, 0                       ; Head idx
  mov dl, [drive]                 ; Drive idx
  mov cl, [sector_offset]         ; Sector idx
  mov bx, SECTOR2                 ; Target pointer

  int 0x13                        ; Sector-based hard drive and floppy disk read and write services

  ; cli
  ; hlt

  ; call read_next_batch          ; N times to read N*127 sectors

  ; call enable_a20                 ; Enable a20 line

  ; xor  eax, eax
  ; xor  ebx, ebx
  ; call get_memsize_64mb           ; Get memory size

  ; mov word [boot_info+multiboot_info.memory_hi], bx
  ; mov word [boot_info+multiboot_info.memory_lo], ax

  ; mov  eax, 0x0
  ; mov  ds, ax
  ; mov  di, MEMMAP
  ; call get_memmap_e820            ; Get memory map

  ; mov [boot_info+multiboot_info.mmap_length], bp
  ; mov dword [boot_info+multiboot_info.mmap_addr], MEMMAP
  ; mov dl, [drive]
  ; mov [boot_info+multiboot_info.boot_device], dl

  ; cli                             ; Switch off interrupts
  ; lgdt [gdt_descriptor]           ; Load gdt descriptor
  ; mov eax, cr0                    ; Store cr0 in eax
  ; or  eax, 0x1                    ; Set first bit
  ; mov cr0, eax                    ; Put eax into cr0

  ; mov ax, DATA_SEG                ; Update segment registers
  ; mov ds, ax
  ; mov ss, ax
  ; mov es, ax
  ; mov fs, ax
  ; mov gs, ax

  ; [Effective Address = Segment*16 + Offset]
  ; mov ax, 0x500
  ; mov ss, ax

  mov sp, 0x5000

  mov eax, [drive]
  mov [DRIVEADDR], eax

  call SECTOR2+4                   ; Skip first 4 bytes, which are size of file

  cli
  hlt

drive:          db 0              ; drive

sector_cnt:     dw 0
sector_offset:  dw 0

times 510-($-$$) db 0             ; Padding
dw 0xaa55                         ; Magic bootable number

SECTOR2: