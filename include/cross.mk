# Cross Compilation

include $(INCLUDE_DIR)/target.mk

TOOLCHAIN_PREFIX=$(TARGET_PREFIX)-elf

export CC           := $(TOOLCHAIN_PREFIX)-gcc
export CXX          := $(TOOLCHAIN_PREFIX)-g++
export LD           := $(TOOLCHAIN_PREFIX)-ld
export AR           := $(TOOLCHAIN_PREFIX)-ar
export RANLIB       := $(TOOLCHAIN_PREFIX)-ranlib
export ASM          := nasm

export CFLAGS       := -ffreestanding -nostdlib -Wall -fpermissive -I$(BUILD_DIR)/include
export CXXFLAGS     := -std=$(CXXSTD) -fno-exceptions -fno-rtti
export LDFLAGS      := -static -nostdlib -nmagic
export AFLAGS       := -f elf32
