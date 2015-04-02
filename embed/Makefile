ECHO = /bin/echo

all: error.html.h listdir.html.h direntry.html.h bootstrap.css.h styles.css.h file.svg.h folder.svg.h

%.h: %
	$(ECHO) \#ifndef\ $(subst .,_,$^) > $@
	$(ECHO) \#define\ $(subst .,_,$^)\ \\ >> $@
	hexdump -e '24/1 "_x%02X" "\n"' $^ | sed 's/_/\\/g; s/.*/    "&"/; s/\\x  //g; s/$$/\\/g;' >> $@
	$(ECHO) >> $@
	$(ECHO) \#endif >> $@

%.css: src/%.css
	cssmin $^ > $@

%.css: src/%.less
	lessc $^ | cssmin > $@

%.html: src/%.html
	html-minifier $^ > $@

%.svg: src/%.svg
	cp $^ $@

clean:
	rm -f *.h
