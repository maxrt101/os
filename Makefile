# os TODO: think of name

.INCLUDE_DIRS += $(pwd)

# Used everywhere
export TOPDIR       := $(shell pwd)
export BINDIR       := $(TOPDIR)/bin
export BUILDDIR     := $(TOPDIR)/build
export LIBDIR       := $(BINDIR)/lib
export INCLUDE_DIR  := $(TOPDIR)/include
export MAKE_LEVEL   := 0

# Aliases
export COMMON       = $(TOPDIR)/include/common.mk
export PACKAGE      = $(TOPDIR)/include/package.mk

include $(COMMON)

# Tools binaries
export KFSCLI       := $(BINDIR)/host/kfscli

# Output files
BOOTSECTOR          := $(BINDIR)/$(CONFIG_BOOTSECTOR_FILE)
BOOTLOADER          := $(BINDIR)/$(CONFIG_BOOTLOADER_FILE)
TARGET              := $(BINDIR)/$(IMAGE_FILE)

$(info build target os:$(PROFILE))

.PHONY: prepare build image package tool run

# @echo "make image $(TARGET)"
image: build
	$(call log,image)
	$(KFSCLI) create $(TARGET)
	$(KFSCLI) setname "$(IMAGE_NAME)" $(TARGET)
	$(KFSCLI) boot $(BOOTSECTOR) $(TARGET)
	$(KFSCLI) write $(BOOTLOADER) /boot $(TARGET)

build: prepare tools packages

tools: prepare
	$(call log,tools)
	make -C tools

packages: prepare
	$(call log,packages)
	make -C package

prepare:
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BUILDDIR)

menuconfig:
	echo "Not implemanted yet"

# @echo "make package $(PKG)"
package:
	make -C package package PKG=$(PKG)

# @echo "make tool $(TOOL)"
tool:
	make -C tools package PKG=$(TOOL)

run:
	@echo "Running image $(TARGET)"
	qemu-system-x86_64 -drive format=raw,file=$(TARGET) -monitor stdio

clean:
	make -C tools clean
	make -C package clean

buildclean:
	rm -rf $(BUILDDIR)

# Silent otput, if V is not defined
$(V).SILENT:
