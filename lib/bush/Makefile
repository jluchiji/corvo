
all: plumber.o puppet.o
	ar rvs $(TOP)/libbush.a *.o

%.o: %.cc
	$(CXX) $(XFLAGS) -o $@ -c -I$(TOP)/include $<

clean:
	rm -rf *.o
