.PHONY: all

all: $(COBJS)

$(COBJS): %.o : %.cc
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $(PKG_BUILD_DIR)/$@