CXX      = g++ -fPIC
DEBUG    = 4
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

TOP     := $(shell pwd)
LIB     := $(TOP)/lib
SRC     := $(TOP)/src
INCLUDE := $(TOP)/include

all: http-serve

http-serve: bin/libbush.a bin/libcorvo.a bin/main.o
	$(CXX) -o $@ bin/main.o bin/*.a

bin/%.o: src/%.cc
	$(CXX) $(XFLAGS) -lpthread -o $@ -c -I$(TOP) $<

bin/libbush.a:
	make -C $(LIB)/bush CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

bin/libcorvo.a:
	make -C $(LIB)/corvo CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

clean:
	rm bin/*
	make clean -C $(LIB)/bush
	make clean -C $(LIB)/corvo
