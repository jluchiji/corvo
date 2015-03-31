
all: path.o plumber.o puppet.o
	ar rvs $(TOP)/libbush.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

clean:
	rm -rf *.o
