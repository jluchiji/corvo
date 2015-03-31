CXX      = g++ -fPIC
DEBUG    = 4
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

TOP     := $(shell pwd)
LIB     := $(TOP)/lib
SRC     := $(TOP)/src
INCLUDE := $(TOP)/include

all: http-serve

http-serve: libbush.a libcorvo.a src/main.o
	$(CXX) -o $@ src/main.o libcorvo.a libbush.a -lpthread

src/%.o: src/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

libbush.a:
	make -C $(LIB)/bush CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

libcorvo.a:
	make -C $(LIB)/corvo CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

clean:
	rm -rf *.a
	rm -rf src/*.o
	make clean -C $(LIB)/bush
	make clean -C $(LIB)/corvo
