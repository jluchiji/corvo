
all: 405.h 404.h

%.h: %.res
	xxd -i $^ > $@

%.res: src/%.html
	inliner $^ > $@

clean:
	rm -f *.h
