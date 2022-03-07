albafetch: albafetch.o, config.h
	gcc albafetch.o -o albafetch

albafetch.o: albafetch.c
	gcc -c albafetch.c

install: albafetch.o, config.h
	gcc albafetch.o -o /usr/bin/albafetch	

clean:
	rm albafetch *.o