# kboot

PKG_NAME            := kboot
PKG_VERSION         := 0.1
PKG_DEPENDENCIES    := kernel-headers libc

include $(COMMON)
include $(PACKAGE)

BOOTSECTOR          := $(CONFIG_BOOTSECTOR_FILE)
TARGET              := $(CONFIG_BOOTLOADER_FILE)

# CXXFLAGS    += -Os -mregparm=3 -fomit-frame-pointer
# CXXFLAGS    += -m16

# export CFLAGS       += -I$(PKG_SRC_DIR)/src
export COBJS        := main.o a20.o mmap.o kboot.o
export AOBJS        := entry.o disk.o pm.o
export OBJS         := $(addprefix $(PKG_BUILD_DIR)/, $(COBJS) $(AOBJS))

define Build/Compile
	make -C src
	$(LD) $(LDFLAGS) -T stage1.ld $(PKG_BUILD_DIR)/boot.o -o $(PKG_BUILD_DIR)/$(BOOTSECTOR)
	$(LD) $(LDFLAGS) -T stage2.ld $(OBJS) -o $(PKG_BUILD_DIR)/$(TARGET)
endef

define Build/Install
	$(call Install/Binary/File, $(PKG_BUILD_DIR)/$(BOOTSECTOR),$(BOOTSECTOR))
	$(call Install/Binary/File, $(PKG_BUILD_DIR)/$(TARGET),$(TARGET))
endef

define Build/Clean
	rm $(OBJS)
endef
