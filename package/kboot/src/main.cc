#include "code16gcc.h"
#include "boot.h"

#include <sys/types.h>
#include <sys/asm.h>
#include <sys/gdt.h>
#include <fs/kfs.h>
#include <string.h>

ASM_SECTION(".text16");

// extern "C" void putc(char);

extern "C" void switch_to_pm();

extern "C" void main() {
  asm("cli\n");
  asm("hlt\n");

  u8 drive = *(u8*)DRIVEADDR;

  enable_a20();

  read_disk(drive, 127, 1, (u8*)DISK_ADDR);

  // int memmap_entries = detect_memory((smap_entry_t*)MEMMAPADDR, 16);

  asm("cli\n");

  setup_gdt();

  switch_to_pm();

  // kfs_disk disk;
  // kfs_parse_data(&disk, (uint8_t*)DISK_ADDR);

  // kfs_node file;
  // if (!kfs_find(&disk, "/boot/boot.cfg", &file)) {
  // }

  asm("cli\n");
  asm("hlt\n");
}


gdt_entry_t gdt[3];

gdt_ptr_t gdt_descriptor;

void setup_gdt() {
  gdt[0].limit_lo = 0;
  gdt[0].base_lo  = 0;
  gdt[0].base_mid = 0;
  gdt[0].type     = 0;
  gdt[0].limit_hi = 0;
  gdt[0].base_hi  = 0;

  // code
  gdt[1].limit_lo = 0xffff;
  gdt[1].base_lo  = 0;
  gdt[1].base_mid = 0;
  gdt[1].type     = 0b10011010;
  gdt[1].limit_hi = 0b11001111;
  gdt[1].base_hi  = 0;

  // data
  gdt[2].limit_lo = 0xffff;
  gdt[2].base_lo  = 0;
  gdt[2].base_mid = 0;
  gdt[2].type     = 0b10010010;
  gdt[2].limit_hi = 0b11001111;
  gdt[2].base_hi  = 0;

  gdt_descriptor.len = sizeof(gdt)-1;
  gdt_descriptor.base = (u32)gdt;

  asm("lgdt %0\n" :: "m"(gdt_descriptor));
}

/*
#include "boot.h"
#include "video.h"
#include "multiboot_info.h"

#include <string.h>
#include <fs/kfs.h>
*/

/*
extern "C" int main(multiboot_info_t* bootinfo) {
  const char* boot_msg = "kboot v" KBOOT_VERSION_STRING " loaded\n";

  puts(boot_msg);

  kfs_disk disk;
  kfs_parse_data(&disk, (uint8_t*)STAGE1_ADDR);

  puts(disk.root.name);
  vga_putch('\n');
  
  kfs_node file;
  if (!kfs_find(&disk, "/test", &file)) {
    puts(file.name);
    vga_putch('\n');
    for (int i = 0; i < file.sub_nodes_count; i++) {
      int block_size = 0;
      memcpy(&block_size, disk.data+file.sub_nodes[i], 4);
      for (int j = 0; j < block_size; j++) {
        vga_putch(*(char*)(disk.data+file.sub_nodes[i]+4+j));
      }
    }
  } else {
    puts("Can't find file\n");
  }


  return 0;
}
*/