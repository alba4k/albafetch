albafetch: src/albafetch.c src/config.h
	gcc src/albafetch.c -o albafetch

install: src/albafetch.c src/config.h
	gcc src/albafetch.c -o /usr/bin/albafetch

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch