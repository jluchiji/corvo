
all: path.o plumber.o puppet.o
	ar rvs $(TOP)/bin/libbush.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP) $<

clean:
	rm *.o
