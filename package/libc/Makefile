# libc

PKG_NAME      := libc
PKG_VERSION   := 0.1

include $(COMMON)
include $(PACKAGE)

TARGET              := $(PKG_BUILD_DIR)/libc.a

export CXXFLAGS      += -I$(call pwd)/include
export COBJS         := int64.o string.o ctype.o
export OBJS          := $(addprefix $(PKG_BUILD_DIR)/, $(COBJS))

define Build/Compile
	make -C src
	$(AR) -rc $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)
endef

define Build/Install
	$(call Install/Folder,include,include)
	$(call Install/File,$(TARGET),lib)
endef

define Build/Clean
	rm $(OBJS)
endef