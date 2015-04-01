CXX      = g++ -fPIC
DEBUG    = 4
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

TOP     := $(shell pwd)
LIB     := $(TOP)/lib

all: clean http-serve

debug: resources libbush.a libcorvo.a libtranspose.a test/main.o
	$(CXX) -o $@ test/*.o libcorvo.a libbush.a libtranspose.a -lpthread

http-serve: resources libbush.a libcorvo.a libtranspose.a src/main.o src/error.o src/serve.o
	$(CXX) -o $@ src/*.o libcorvo.a libbush.a libtranspose.a -lpthread

src/%.o: src/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

test/%.o: test/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

libbush.a:
	make -C $(LIB)/bush CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

libcorvo.a:
	make -C $(LIB)/corvo CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

libtranspose.a:
	make -C $(LIB)/transpose CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

.PHONY: resources
resources:
	make -C $(TOP)/embed

clean:
	rm -f http-serve debug
	rm -f *.a
	rm -f src/*.o
	make clean -C $(LIB)/bush
	make clean -C $(LIB)/corvo
	make clean -C ./embed
