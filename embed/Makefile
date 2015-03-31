
all: clean grid.h jquery.h navbar.h normalize.h style.h
	echo \#ifndef\ _EMBED_H_ >  embed.h
	echo \#define\ _EMBED_H_ >> embed.h
	cat embed.tmp >> embed.h
	echo \#endif >> embed.h
	rm -f embed.tmp

%.h: %.res
	echo "#include \"embed/$@\"" >> embed.tmp
	xxd -i $^ > $@

%.res: src/%.html
	html-minifier $^ > $@

clean:
	rm -f *.h
