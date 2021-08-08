#ifndef _KBOOT_MULTIBOOT_INFO_H_
#define _KBOOT_MULTIBOOT_INFO_H_

#include <sys/types.h>

typedef struct {
  u32 flags;
  u32 memory_lo;
  u32 memory_hi;
  u32 boot_device;
  u32	cmdline;
  u32	mods_count;
  u32	mods_addr;
  u32	syms0;
  u32	syms1;
  u32	syms2;
  u32	mmap_length;
  u32	mmap_addr;
  u32	drives_length;
  u32	drives_addr;
  u32	config_table;
  u32	bootloader_name;
  u32	apm_table;
  u32	vbe_control_info;
  u32	vbe_mode_info;
  u16	vbe_mode;
  u32	vbe_interface_addr;
  u16	vbe_interface_len;
} multiboot_info_t;

#endif