all: request.o response.o server.o util.o
	ar rvs $(TOP)/bin/libcorvo.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<


clean:
	rm *.o
