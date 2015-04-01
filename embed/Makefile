ECHO = /bin/echo

all: error.html.h listdir.html.h pure.css.h styles.css.h

%.css.h: %.css
	$(ECHO) \#ifndef\ $(subst .,_,$^) > $@
	$(ECHO) \#define\ $(subst .,_,$^)\ \\ >> $@
	hexdump -e '24/1 "_x%02X" "\n"' $^ | sed 's/_/\\/g; s/.*/    "&"/; s/\\x  //g; s/$$/\\/g;' >> $@
	$(ECHO) >> $@
	$(ECHO) \#endif >> $@

%.css: src/%.css
	cssmin $^ > $@

%.html.h: %.html
	$(ECHO) \#ifndef\ $(subst .,_,$^) > $@
	$(ECHO) \#define\ $(subst .,_,$^)\ \\ >> $@
	hexdump -e '16/1 "_x%02X" "\n"' $^ | sed 's/_/\\/g; s/.*/    "&"/; s/\\x  //g; s/$$/\\/g;' >> $@
	$(ECHO) >> $@
	$(ECHO) \#endif >> $@

%.html: src/%.html
	html-minifier $^ > $@

clean:
	rm -f *.h
