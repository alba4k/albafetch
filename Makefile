albafetch: src/albafetch.c src/config.h
	gcc src/albafetch.c -o albafetch

install: src/albafetch.c src/config.h
	gcc src/albafetch.c -o /usr/bin/albafetch	

isntall: install		# cause I can't type

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch