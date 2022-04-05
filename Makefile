albafetch: src/albafetch.c src/config.h src/logos.h
	gcc -o albafetch src/albafetch.c

test: test.c
	gcc test.c -o test

install: src/albafetch.c src/config.h albafetch
	cp albafetch /usr/bin/albafetch

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch test
