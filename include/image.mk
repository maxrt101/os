# image.mk

include $(COMMON)

ifeq ("$(CONFIG_IMAGE_NAME)","")
IMAGE_NAME=$(CONFIG_IMAGE_NAME)
else
IMAGE_FILE=$(DEFAULT_IMAGE_NAME)
endif

ifeq ("$(CONFIG_PROFILE)","")
PROFILE=$(CONFIG_PROFILE)
else
PROFILE=$(DEFAULT_PROFILE)
endif

IMG_BOOTSECTOR := $(BINDIR)/$(CONFIG_BOOTSECTOR_FILE)
IMG_BOOTLOADER := $(BINDIR)/$(CONFIG_BOOTLOADER_FILE)
IMG_FILE       := $(IMAGE_PREFIX)-$(IMAGE_NAME).kfs
IMG_TARGET     := $(BINDIR)/$(IMAGE_FILE)

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
	$(call Image/Create,$(IMG_TARGET),$(IMG_NAME),$(IMG_BOOTSECTOR),$(IMG_BOOTLOADER))
endef
