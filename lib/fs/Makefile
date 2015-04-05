all: path.o fileinfo.o
	ar rvs $(TOP)/libfs.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

clean:
	rm -rf *.o
