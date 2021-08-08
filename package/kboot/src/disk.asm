bits 16
section .text16

global read_disk

read_disk:                        ; void read_disk(int drive, int sector_count, int sector_offset, u8* target_ptr)
  mov ah, 0x2                     ; Read sectors
  mov ch, 0                       ; Cylinder idx
  mov dh, 0                       ; Head idx
  mov dl, [esp+4]                 ; Drive idx
  mov al, [esp+8]                 ; Sectors to read

  mov cl, [esp+12]                ; Sector idx
  mov bx, [esp+16]                ; Target pointer

  int 0x13                        ; Sector-based hard drive and floppy disk read and write services
  ret