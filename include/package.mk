# pacakge.mk

include $(INCLUDE_DIR)/build.mk
include $(INCLUDE_DIR)/target.mk

ifeq ("$(PKG_HOST)","1")
include $(INCLUDE_DIR)/host.mk
else
include $(INCLUDE_DIR)/cross.mk
endif

export PKG_BUILD_DIR := $(BUILD_DIR)/pkg/$(PKG_NAME)
export PKG_SRC_DIR   := $(TOPDIR)/$(PKG_DIR_NAME)/$(PKG_NAME)

# Internal functions
define Build/ExternalDependency
	$(if $(V), $(info build $(PKG_NAME) dependency $(1)/$(2)))
	make -C $(1)/$(2)
endef

define Build/Dependency
	$(if $(V), $(info build $(PKG_NAME) dependency $(PACKAGES_DIR)/$(1)))
	make -C $(PACKAGES_DIR)/$(1)
endef

define Build/Dependencies
	$(foreach pkg, $(PKG_DEPENDENCIES), $(call Build/Dependency,$(pkg)))
endef

define Build/CheckIfBuilt
	check() {\
		for file in $$1/*; do \
			if [ -d $$file ]; then \
				check $$file; \
			else \
				if [[ ! $$file == *.built ]] && [ -f $(PKG_BUILD_DIR)/.built ]; then \
					if [ "`date -r $$file +%s`" -gt "`cat $(PKG_BUILD_DIR)/.built`" ]; then \
						rm $(PKG_BUILD_DIR)/.built; \
						exit 0; \
					fi; \
				fi; \
			fi; \
		done; \
	}; check "$(1)";
endef

# echo "build package $(PKG_NAME)"; \
# echo "FORCE build $(PKG_NAME)"; \
# echo "package $(PKG_NAME) is already built";
define Build/Build
	do_build() { \
		if [ ! -f "$(PKG_BUILD_DIR)/.built" ]; then \
			make build; \
		elif [ ! -z "$(FORCE)" ]; then \
			make build; \
		fi; \
	}; do_build
endef

# Authomatic Compile
define Build/Compile/C
	do_compile() { \
		for FILE in $$1; do \
			$(CC) -c $(CFLAGS) $$FILE -o "$(PKG_BUILD_DIR)/$${FILE%.*}.o"; \
		done; \
	}; do_compile "$1";
	$(CC) $(LDFLAGS) $(addprefix $(PKG_BUILD_DIR)/, $(addsuffix .o, $(basename $1))) -o $2
endef

define Build/Compile/CPP
	do_compile() { \
		for FILE in $$1; do \
			$(CXX) -c $(CFLAGS) $(CXXFLAGS) $$FILE -o "$(PKG_BUILD_DIR)/$${FILE%.*}.o"; \
		done; \
	}; do_compile "$1";
	$(CXX) $(LDFLAGS) $(addprefix $(PKG_BUILD_DIR)/, $(addsuffix .o, $(basename $1))) -o $2
endef

# Defaults
define Build/Clean/Default
	rm $(PKG_BUILD_DIR)/*.o
endef

# Targets
.PHONY: check-rebuild

check-rebuild:
	$(call log, PKG $(PKG_DIR_NAME)/$(PKG_NAME))
	$(call Build/CheckIfBuilt, $(shell pwd))
	$(call Build/Build)

build: install

install: compile
	$(call Build/Install)
	date +%s > $(PKG_BUILD_DIR)/.built
	rm -rf $(PKG_BUILD_DIR)/.rebuild

compile: configure
	$(call Build/Compile)

configure: prepare
	$(call Build/Configure)

prepare:
	mkdir -p $(PKG_BUILD_DIR)
	$(call Build/Dependencies)
	$(call Build/Prepare)

clean-build: FORCE
	rm -rf $(PKG_BUILD_DIR)

clean: clean-build
	$(call Build/Clean)
