# target.mk

include $(COMMON)

include $(INCLUDE_DIR)/defaults.mk

ifeq ("$(CONFIG_TARGET)","")
TARGET_NAME=DEFAULT_TARGET_NAME
else
TARGET_NAME=$(CONFIG_TARGET)
endif
