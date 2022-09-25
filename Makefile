.PHONY: build/albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

OS := $(shell uname -s 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

ifeq ($(OS),Linux)
	OBJ := info.o main.o queue.o utils.o
	INSTALLPATH := /usr/bin
	ifeq ($(PACMAN),/bin/pacman)
		INCLUDE := -l alpm
		ARCH_BASED := -D ARCH_BASED
	endif
endif

ifeq ($(OS),Darwin)
	INSTALLPATH := /usr/local/bin
	OBJ := info.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	INCLUDE := -framework Foundation -framework IOKit
endif

build/$(TARGET): $(OBJ)
	mkdir -p build/
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) $(CFLAGS)

main.o: src/main.c src/vars.h src/logos.h src/info.h
	$(CC) -c src/main.c

info.o: src/info.c src/vars.h src/info.h
	$(CC) -c src/info.c $(ARCH_BASED)

utils.o: src/utils.c
	$(CC) -c src/utils.c

bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c

macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c manually compiled as specified in the package README.md. It should work on both x86_64 ("Intel") and arm64 ("Apple Silicon

queue.o: src/queue.c
	$(CC) -c src/queue.c

macos_gpu_string.o: src/macos_gpu_string.m
	$(CC) -c src/macos_gpu_string.m $(INCLUDE)

run: build/$(TARGET)
	build/$(TARGET)

install: build/$(TARGET)
	cp -f build/$(TARGET) $(DESTDIR)$(INSTALLPATH)/$(TARGET)

uninstall:
	rm $(INSTALLPATH)/$(TARGET)

clean:
	-rm -rf build test *.o
