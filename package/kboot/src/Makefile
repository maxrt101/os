.PHONY: all

all: $(COBJS) $(AOBJS)
	$(ASM) $(AFLAGS) boot.asm -o $(PKG_BUILD_DIR)/boot.o

$(COBJS): %.o : %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $(PKG_BUILD_DIR)/$@

$(AOBJS): %.o : %.asm
	$(ASM) $(AFLAGS) $< -o $(PKG_BUILD_DIR)/$@