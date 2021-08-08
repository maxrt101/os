# Host Compilation
export CC           := gcc
export CXX          := g++
export LD           := ld
export AR           := ar
export RANLIB       := ranlib

export CFLAGS       :=
export CXXFLAGS     := $(CFLAGS) -std=$(CXXSTD)
