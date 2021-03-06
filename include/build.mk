# build.mk

ifeq ("$(CONFIG_PROFILE)","")
PROFILE=$(DEFAULT_PROFILE)
else
PROFILE=$(CONFIG_PROFILE)
endif

ifeq ("$(CONFIG_PROFILE)","dev")
IMAGE_PREFIX=dev
endif

ifeq ("$(CONFIG_CXXSTD)","")
CXXSTD=c++11
else
CXXSTD=$(CONFIG_CXXSTD)
endif

# Installs
define Install/File
	cp $(1) $(BUILD_DIR)/$(2)
endef

define Install/Image/File
	cp $(1) $(ROOTFS_DIR)/$(2)
endef

define Install/Host/File
	cp $(1) $(HOST_DIR)/$(2)
endef

define Install/Folder
	cp -r $(1) $(BUILD_DIR)/$(2)
endef

define Install/Image/Folder
	cp -r $(1) $(ROOTFS_DIR)/$(2)
endef

define Install/Host/Folder
	cp -r $(1) $(HOST_DIR)/$(2)
endef

define Install/Binary/File
	cp $(1) $(BINDIR)/$(2)
endef
