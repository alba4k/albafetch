albafetch: albafetch.o info.o
	gcc -o albafetch albafetch.o info.o

albafetch.o: src/main.c src/config.h src/info.h src/vars.h
	gcc -c src/main.c

info.o: src/info.c src/config.h src/info.h src/vars.h
	gcc -c src/info.c

test: test.c
	gcc test.c -o test

install: src/main.c src/config.h albafetch
	cp albafetch /usr/bin/albafetch

uninstall:
	rm /usr/bin/albafetch

clean:
	rm albafetch test *.o
