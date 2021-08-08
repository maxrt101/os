# common.mk

include $(TOPDIR)/.config

IMAGE_NAME=$(CONFIG_IMAGE_NAME)
PROFILE=$(CONFIG_PROFILE)
CXXSTD=$(CONFIG_CXXSTD)

ifeq ("$(PROFILE)","DEV")
IMAGE_PREFIX=dev
endif

IMAGE_FILE=$(IMAGE_PREFIX)-termos.kfs

ifeq ("$(CONFIG_BUILD_DEBUG)","y")
	BUILD_DEBUG=1
endif

ifeq ("$(BUILD_DEBUG)","1")
export SHELL = /bin/bash -x
endif

MAKE_LEVEL := $(shell echo "$(MAKE_LEVEL)+1" | bc)
# $(info make level: $(MAKE_LEVEL))
# log = $(info make[$(MAKE_LEVEL)]$(shell printf ' %.0s' {1..$(MAKE_LEVEL)})$(1))
log = $(info make[$(MAKE_LEVEL)] $(1))
# $(call log, TEST)
