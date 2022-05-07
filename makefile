.PHONY: albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch
OBJ := info.o main.o
OBJ_OSX := macos_infos.o bsdwrap.o
SRC := src/info.c src/main.c
SRC_OSX := macos_infos.c bsdwrap.c
INCLUDE := -I src

build/albafetch: $(OBJ_OSX) $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(OBJ_OSX) $(CFLAGS) || $(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(CFLAGS)

main.o: src/main.c src/config.h src/vars.h src/logos.h src/info.h
	$(CC) -c src/main.c

info.o: src/info.c src/config.h src/vars.h src/info.h
	$(CC) -c src/info.c

bsdwrap.o: src/bsdwrap.c
	-$(CC) -c src/bsdwrap.c

macos_infos.o: src/macos_infos.c
	-$(CC) -c src/macos_infos.c

run: $(OBJ_OSX) $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(OBJ_OSX) $(CFLAGS) || $(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(CFLAGS) && build/$(TARGET)

install: build/$(TARGET)
	cp build/$(TARGET) /usr/bin/$(TARGET)

uninstall:
	rm /usr/bin/$(TARGET)

test: test.c
	$(CC) -o build/test test.c && build/test

clean:
	-rm -rf build test *.o