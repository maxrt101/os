#include "code16gcc.h"
#include "boot.h"

#include <sys/asm.h>

ASM_SECTION(".latetext16");

int /*__attribute__((noinline)) __attribute__((regparm(3)))*/ detect_memory(smap_entry_t* buffer, int max_entries) {
  u32 cont_id = 0;
  int entries = 0, signature, bytes;
  do {
    asm volatile(
      "int $0x15" 
      : "=a"(signature), "=c"(bytes), "=b"(cont_id)
      : "a"(0xE820), "b"(cont_id), "c"(24), "d"(0x534D4150), "D"(buffer)
    );
    if (signature != 0x534D4150) {
      return -1; // error
    }
    if (bytes > 20 && (buffer->acpi & 0x0001) == 0) {
      // ignore this entry
    } else {
      buffer++;
      entries++;
    }
  } while (cont_id != 0 && entries < max_entries);
  return entries;
}

// void get_memory_size(smap_entry_t* entries, int entries_count, int* total_memory, int* usable_memory) {}