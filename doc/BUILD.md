# Build System

### 1. Common Structure
Code that represents some functionality is a package(module).  
Packages are located in `package/` directory. Every package has a `Makefile`.  

### 2. Makefile Structure
Each package's `Makefile` must define `PKG_NAME` and `PKG_VERSION`, and include `common.mk`.  
You can specify dependencies through `PKG_DEPENDENCIES` (space separated list).  
Building has 4 stages - prepare, configure, compile and install.  
Also there is clean target. 
Common `Makefile` for a package has following structure:  
```
# package name

PKG_NAME      := test # package name
PKG_VERSION   := 0.1  # package version

include $(COMMON)  # common definitions, also includes .config
include $(PACKAGE) # package structure definitions

TARGET        := $(BINDIR)/test # target binary file

define Build/Prepare
  # commands
endef

define Build/Configure
  # commands
endef

define Build/Compile
  # commands
endef

define Build/Install
  # commands
endef

define Build/Clean
  # commands
endef

```
Sample `Makefile` fot package `test`:  
```
# package test

PKG_NAME      := test
PKG_VERSION   := 0.1

include $(COMMON)
include $(PACKAGE)

TARGET        := $(BINDIR)/test

CSRC          := test.c main.c
OBJS          := $(addprefix $(PKG_BUILD_DIR), $(COBJS))

define Compile/C
  $(CC) $(CFLAGS) $1 -o $2
endif

define Build/Compile
  $(foreach file, $(CSRC), $(call Compile/C,$(file),)
endef
```
### 3. Building
To build image, type `make` at topdir.  
All packages under `package/` are built authomatically.
If you want to build specific package run `make package PKG=PACKAGE_NAME` from topdir. If output says `package PACKAGE_NAME is already built`, add `FORCE=1` to the command above.
### 4. Useful Makefile Varibles
`TOPDIR` - top directory  
`BINDIR` - directory for binaries  
`BUILDDIR` - directory for build files  
`COMMON` - alias for `$(TOPDIR)/include/common.mk`  
`PACKAGE` - alias for `$(TOPDIR)/include/package.mk`  
`PKG_BUILDDIR` - build directory for package, present only if `PACKAGE` is included  
### 5. Debug
Use `V=1` to enable debug. 
Set `CONFIG_BUILD_DEBUG` to `y` in `.config` to enable shell debug.  
### 6. Config
Configuration is stored in `.config` file.