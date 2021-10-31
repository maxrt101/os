# Host Compilation

export CC           := gcc
export CXX          := g++
export LD           := ld
export AR           := ar
export RANLIB       := ranlib

export CFLAGS       := -I$(HOST_DIR)/include
export CXXFLAGS     := $(CFLAGS) -std=$(CXXSTD)
export LDFLAGS      := -L$(HOST_DIR)/lib
