albafetch: albafetch.c config.h
	gcc albafetch.c -o albafetch

install: albafetch.c config.h
	gcc albafetch.c -o /usr/bin/albafetch	

clean:
	rm albafetch