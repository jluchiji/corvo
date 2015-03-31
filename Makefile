CXX      = g++ -fPIC
DEBUG    = 3
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

TOP     := $(shell pwd)
LIB     := $(TOP)/lib

all: http-serve

http-serve: libbush.a libcorvo.a src/main.o src/error.o src/serve.o
	$(CXX) -o $@ src/*.o libcorvo.a libbush.a -lpthread

embedded:
	make -C $(TOP)/embed

src/%.o: src/%.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

libbush.a:
	make -C $(LIB)/bush CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

libcorvo.a:
	make -C $(LIB)/corvo CXX="$(CXX)" XFLAGS="$(XFLAGS)" DEBUG=$(DEBUG) TOP=$(TOP)

clean:
	rm -f http-serve
	rm -f *.a
	rm -f src/*.o
	make clean -C $(LIB)/bush
	make clean -C $(LIB)/corvo
