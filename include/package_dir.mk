export PACKAGES_DIR := $(shell pwd)

# include $(COMMON)

SUBDIRS := $(wildcard */)

.PHONY: all prepare $(SUBDIRS)

all: prepare $(SUBDIRS)

prepare:
	$(call log, DIR $(PKG_DIR_NAME))

$(SUBDIRS):
	make -C $@

package:
	make -C $(PKG)
