ECHO = /bin/echo

all: pure.css.h styles.css.h error.html.h

%.css.h: %.css
	xxd -i $^ > $@

%.css: src/%.css
	cssmin $^ > $@

%.html.h: %.html
	xxd -i $^ > $@

%.html: src/%.html
	html-minifier $^ > $@

clean:
	rm *.h
