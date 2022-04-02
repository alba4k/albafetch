albafetch: src/albafetch.c src/config.h src/logos.h
	gcc src/albafetch.c -o albafetch
	touch ~/.albafetch.log

install: src/albafetch.c src/config.h albafetch
	cp albafetch /usr/bin/albafetch

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch