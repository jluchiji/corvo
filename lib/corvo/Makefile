all: request.o response.o server.o util.o
	ar rvs $(TOP)/libcorvo.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<


clean:
	rm -rf *.o
