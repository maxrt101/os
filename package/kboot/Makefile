# kboot

PKG_NAME            := kboot
PKG_VERSION         := 0.1

include $(COMMON)
include $(PACKAGE)

BOOTSECTOR          := $(TOPDIR)/$(CONFIG_BOOTSECTOR_FILE)
TARGET              := $(TOPDIR)/$(CONFIG_BOOTLOADER_FILE)

# CXXFLAGS    += -Os -mregparm=3 -fomit-frame-pointer
# CXXFLAGS    += -m16

export COBJS        := main.o a20.o mmap.o kboot.o
export AOBJS        := entry.o disk.o pm.o
export OBJS         := $(addprefix $(PKG_BUILD_DIR)/, $(COBJS) $(AOBJS))

define Build/Compile
	make -C src
	$(LD) $(LDFLAGS) -T stage1.ld $(PKG_BUILD_DIR)/boot.o -o $(BOOTSECTOR)
	$(LD) $(LDFLAGS) -T stage2.ld $(OBJS) -o $(TARGET)
endef

define Build/Clean
	rm $(OBJS)
endef
