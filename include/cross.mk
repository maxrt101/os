# Cross Compilation
export CC           := i386-elf-gcc
export CXX          := i386-elf-g++
export LD           := i386-elf-ld
export AR           := i386-elf-ar
export RANLIB       := i386-elf-ranlib
export ASM          := nasm

export CFLAGS       := -ffreestanding -nostdlib -Wall -fpermissive -I$(TOPDIR)/include -I$(TOPDIR)/package/libc/include
export CXXFLAGS     := $(CFLAGS) -std=$(CXXSTD) -fno-exceptions -fno-rtti
export LDFLAGS      := -static -nostdlib -nmagic
export AFLAGS       := -f elf32
