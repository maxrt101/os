# common.mk

ifeq ("$(TOPDIR)","")
	$(error This Makefile shouldn't be run directly)
endif

include $(TOPDIR)/.config

ifeq ("$(CONFIG_BUILD_DEBUG)","1")
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
