.PHONY: all

all: $(COBJS)

$(COBJS): %.o : %.cc
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(INCLUDES) $< -o $(PKG_BUILD_DIR)/$@