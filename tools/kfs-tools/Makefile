# kfs-tools

PKG_NAME      := kfs_tools
PKG_VERSION   := 0.1
PKG_HOST      := 1

include $(COMMON)
include $(PACKAGE)

TARGET    := $(PKG_BUILD_DIR)/kfscli
CXXFLAGS	+= -Iinclude
SRC       := main.cc kfs.cc

# $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
define Build/Compile
	$(call Build/Compile/CPP,$(SRC),$(TARGET))
endef

define Build/Install
	$(call Install/Host/File,$(TARGET),bin)
	$(call Install/Host/Folder,include,)
	$(call Install/Folder,include,)
endef
