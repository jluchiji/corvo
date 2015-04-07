all: buffer.o fragment.o
	ar rvs $(TOP)/libtranspose.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP)/include $<


clean:
	rm -rf *.o
