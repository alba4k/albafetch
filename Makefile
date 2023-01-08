.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

OS := $(shell uname -s 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

ifeq ($(OS),Linux)
	OBJ := info.o main.o queue.o utils.o
	INSTALLPATH := /usr/bin
	INCLUDE := -l curl -l pci
endif

ifeq ($(OS),Darwin)
	INSTALLPATH := /usr/local/bin
	OBJ := info.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	INCLUDE := -framework Foundation -framework IOKit -l curl
endif

build/$(TARGET): $(OBJ)
	mkdir -p build/
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(CFLAGS)

main.o: src/main.c src/vars.h src/logos.h src/info.h
	$(CC) -c src/main.c $(CFLAGS)

info.o: src/info.c src/vars.h src/info.h
	$(CC) -c src/info.c $(CFLAGS)

utils.o: src/utils.c
	$(CC) -c src/utils.c $(CFLAGS)

bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c $(CFLAGS)

macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c $(CFLAGS)

queue.o: src/queue.c
	$(CC) -c src/queue.c $(CFLAGS)

macos_gpu_string.o: src/macos_gpu_string.m
	$(CC) -c src/macos_gpu_string.m $(CFLAGS)

run: build/$(TARGET)
	build/$(TARGET)

install: build/$(TARGET)
	cp -f build/$(TARGET) $(DESTDIR)$(INSTALLPATH)/$(TARGET)

uninstall:
	rm $(INSTALLPATH)/$(TARGET)

clean:
	-rm -rf build *.o
