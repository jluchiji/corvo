CXX      = g++ -fPIC
DEBUG    = 3
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

TOP     := $(shell pwd)
LIB     := $(TOP)/lib

all: clean httpd

debug: resources libbush.a libcorvo.a libtranspose.a libfs.a test/main.o
	$(CXX) -o $@ test/*.o *.a -lpthread

httpd: resources libbush.a libcorvo.a libtranspose.a libfs.a src/main.o src/error.o src/serve.o
	$(CXX) -o $@ src/*.o *.a -lpthread

src/%.o: src/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) -I. $<

test/%.o: test/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) -I. $<

%.a:
	@echo Building $@...
	@make -s -C $(LIB)/$(shell sed 's/^lib//g; s/\.a$$//g;' <<< $@) CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

.PHONY: resources
resources:
	make -C $(TOP)/embed

clean:
	rm -f httpd debug
	rm -f *.a
	rm -f src/*.o
	make clean -C $(LIB)/bush
	make clean -C $(LIB)/corvo
	make clean -C ./embed
