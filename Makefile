CXX      = g++ -fPIC
LIB      = -lpthread
DEBUG    = 4
CFLAGS   = -g
XFLAGS   = $(CFLAGS) -DDEBUG=$(DEBUG)

all: http-serve

http-serve: main.o request.o response.o server.o util.o
	$(CXX) -o http-serve *.o $(LIB)

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I. $<

clean:
	rm -f *.o http-serve
