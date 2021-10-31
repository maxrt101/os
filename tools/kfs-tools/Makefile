# kfs-tools

PKG_NAME      := kfs_tools
PKG_VERSION   := 0.1

include $(COMMON)
include $(PACKAGE)

TARGET    := $(PKG_BUILD_DIR)/kfscli
CXXFLAGS	+= -I$(call pwd)/include
SRC       := main.cc kfs.cc

define Build/Compile
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
endef

define Build/Install
	$(call Install/Host/File,$(TARGET),$(HOST_DIR)/bin)
	$(call Install/Host/Folder,include,include)
	$(call Install/Folder,include,include)
endef
