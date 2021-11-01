# os TODO: think of name

.INCLUDE_DIRS += $(pwd)

# Used everywhere
export TOPDIR       := $(shell pwd)
export BINDIR       := $(TOPDIR)/bin
export INCLUDE_DIR  := $(TOPDIR)/include
export BUILD_DIR    := $(TOPDIR)/build
export ROOTFS_DIR   := $(BUILD_DIR)/root
export HOST_DIR     := $(BINDIR)/host

export MAKE_LEVEL   := 0

# Aliases
export COMMON       = $(INCLUDE_DIR)/common.mk
export PACKAGE      = $(INCLUDE_DIR)/package.mk

# Includes
include $(COMMON)
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/target.mk

$(info build target os:$(PROFILE))

.PHONY: prepare build image package tool run

# @echo "make image $(TARGET)"
image: build
	$(call Image/Create/Default)

build: prepare tools packages

tools: prepare
	$(call log,tools)
	make -C tools

packages: prepare
	$(call log,packages)
	make -C package

prepare:
	mkdir -p $(BINDIR)
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(BUILD_DIR)/include
	mkdir -p $(BUILD_DIR)/lib
	mkdir -p $(ROOTFS_DIR)
	mkdir -p $(HOST_DIR)
	mkdir -p $(HOST_DIR)/bin
	mkdir -p $(HOST_DIR)/include
	mkdir -p $(HOST_DIR)/lib

menuconfig:
	echo "Not implemanted yet"

# @echo "make package $(PKG)"
package:
	make -C package package PKG=$(PKG)

# @echo "make tool $(TOOL)"
tool:
	make -C tools package PKG=$(TOOL)

run:
	@echo "Running image $(IMG_FILE)"
	qemu-system-x86_64 -drive format=raw,file=$(IMG_FILE) -monitor stdio

clean:
	make -C tools clean
	make -C package clean

buildclean:
	rm -rf $(BUILD_DIR)

# Silent otput, if V is not defined
$(V).SILENT:
