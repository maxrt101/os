#ifndef _KBOOT_BOOT_H_
#define _KBOOT_BOOT_H_

#include <sys/types.h>

#define KBOOT_VERSION_STRING  "0.1"

#define SECTOR_SIZE           512
#define STAGE1_ADDR           0x7c00
#define STAGE2_ADDR           0x7e04
#define DISK_ADDR             0xf000

#define DRIVEADDR             0x1000
#define MEMMAPADDR            0x1000

/* Basic port io */
inline u8 inb(u16 port) {
  u8 result;
  __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
  return result;
}

inline void outb(u16 port, u8 data) {
  __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

inline u16 inw(u16 port) {
  u16 result;
  __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
  return result;
}

inline void outw(u16 port, u16 data) {
  __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

/* disk.asm */
extern "C" void read_disk(int drive_number, int sector_count, int sector_offset, u8* target_ptr);

/* a20.cc */
void enable_a20();

/* mmap.cc */
struct smap_entry_t {
  u32 base_lo;     // base address uint64_t
  u32 base_hi;
  u32 length_lo;   // length uint64_t
  u32 length_hi;
  u32 type;        // entry type
  u32 acpi;        // extended
}__attribute__((packed));
int detect_memory(smap_entry_t* buffer, int max_entries);

/* main.cc */
void setup_gdt();

#endif