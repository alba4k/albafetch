.PHONY: albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch
SRC1 := src/main.c
SRC2 := src/info.c
OBJ := info.o main.o macos_infos.o bsdwrap.o
INCLUDE := -I src

build/albafetch: $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ)

main.o: $(SRC1) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC1)

info.o: $(SRC2) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC2)

bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c

macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c

test: test.c
	$(CC) -o test test.c && ./test

linux: main.o info.o
	mkdir -p build
	$(CC) src/info.c src/main.c -o build/linux

install:
	mkdir -p build
	cp build/linux /usr/bin/$(TARGET) || cp build/$(TARGET) /usr/bin/$(TARGET)

uninstall:
	rm /usr/bin/$(TARGET)

run: $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) && build/$(TARGET)

clean:
	rm -rf build test *.o
