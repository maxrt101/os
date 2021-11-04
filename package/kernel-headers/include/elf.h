#ifndef _ELF_H_
#define _ELF_H_

#include <stdint.h>

#define ELF_MAGIC 0x464C457FU

#define ELF_TYPE_NONE 0
#define ELF_TYPE_REL  1
#define ELF_TYPE_EXEC 2
#define ELF_TYPE_DYN  3
#define ELF_TYPE_CORE 4

#define ELF_ARCH_X86 3

#endif