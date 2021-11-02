# image.mk

ifeq ("$(CONFIG_IMAGE_NAME)","")
IMG_NAME=$(DEFAULT_IMAGE_NAME)
else
IMG_NAME=$(CONFIG_IMAGE_NAME)
endif

IMG_BOOTSECTOR := $(BINDIR)/$(CONFIG_BOOTSECTOR_FILE)
IMG_BOOTLOADER := $(BINDIR)/$(CONFIG_BOOTLOADER_FILE)
IMG_FILE_NAME  := $(PROFILE)-$(IMG_NAME).kfs
IMG_FILE       := $(BINDIR)/$(IMG_FILE_NAME)

KFSCLI=$(HOST_DIR)/bin/kfscli

# 1 - Image File
# 2 - Image Name
# 3 - Bootsector File
# 4 - Bootloader File
define Image/Create
	$(call log,image)
	$(KFSCLI) create $(1)
	$(KFSCLI) setname "$(2)" $(1)
	$(KFSCLI) boot $(3) $(1)
	$(KFSCLI) write $(4) /boot $(1)
endef

define Image/Create/Default
	$(call Image/Create,$(IMG_FILE),$(IMG_NAME),$(IMG_BOOTSECTOR),$(IMG_BOOTLOADER))
endef
