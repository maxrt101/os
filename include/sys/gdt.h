#ifndef _SYS_GDT_H_
#define _SYS_GDT_H_

#include <sys/types.h>

#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed

struct gdt_entry_t {
  u16 limit_lo;   // limit (bits 0-15)
  u16 base_lo;    // base  (bits 0-15)
  u8  base_mid;   // base  (bits 0-15)
  u8  type;       // 1st flags, type flags
  u8  limit_hi;   // 2nd flags, limit (bits 16-19)
  u8  base_hi;    // base (bits 24-31)
};

struct gdt_ptr_t {
  u16 len;
  u32 base;
};

#endif