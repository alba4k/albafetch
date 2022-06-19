.PHONY: build/albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch

OS := $(shell uname -s)
PACMAN := $(shell ls /bin/pacman)

ifeq ($(OS),Linux)
	OBJ := info.o main.o queue.o
	ifeq ($(PACMAN),/bin/pacman)
		INCLUDE := -l alpm
		ARCH_BASED := -D ARCH_BASED
	endif
endif

ifeq ($(OS),Darwin)
	OBJ := info.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o
	INCLUDE := -framework Foundation -framework IOKit
endif

build/$(TARGET): $(OBJ)
	mkdir -p build
	mkdir -p ~/.config
	touch ~/.config/$(TARGET).conf
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(CFLAGS)

main.o: src/main.c src/vars.h src/logos.h src/info.h
	$(CC) -c src/main.c

info.o: src/info.c src/vars.h src/info.h
	$(CC) -c src/info.c $(ARCH_BASED)

bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c

macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c

queue.o: src/queue.c
	$(CC) -c src/queue.c

macos_gpu_string.o: src/macos_gpu_string.m
	$(CC) -c src/macos_gpu_string.m $(INCLUDE)

run: build/$(TARGET)
	build/$(TARGET)

install: build/$(TARGET)
	cp -f build/$(TARGET) $(DESTDIR)/usr/bin/$(TARGET)

uninstall:
	rm /usr/bin/$(TARGET)

clean:
	-rm -rf build test *.o
