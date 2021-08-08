# kfs-tools

PKG_NAME      := kfs_tools
PKG_VERSION   := 0.1

include $(COMMON)
include $(PACKAGE)

TOOLSDIR  := $(BINDIR)/host
TARGET    := $(TOOLSDIR)/kfscli
CXXFLAGS	+= -I$(TOPDIR)/include
SRC       := main.cc kfs.cc

define Build/Prepare
	mkdir -p $(TOOLSDIR)
endef

define Build/Compile
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
endef
