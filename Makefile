albafetch: src/albafetch.c src/config.h src/logos.h
	gcc src/albafetch.c -o albafetch
	touch ~/.albafetch.log

test: src/test.c
	gcc src/test.c -o test

install: src/albafetch.c src/config.h albafetch
	cp albafetch /usr/bin/albafetch

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch test