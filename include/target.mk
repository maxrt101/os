# target.mk

# include $(COMMON)

include $(INCLUDE_DIR)/defaults.mk

ifeq ("$(CONFIG_TARGET)","")
TARGET_PREFIX=DEFAULT_TARGET_NAME
else
TARGET_PREFIX=$(CONFIG_TARGET)
endif
