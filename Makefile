.PHONY: build/albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

OS := $(shell uname -s 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

ifeq ($(OS),Linux)
	OBJ := info.o main.o queue.o
	ifeq ($(PACMAN),/bin/pacman)
		INCLUDE := -l alpm
		ARCH_BASED := -D ARCH_BASED
	endif
endif

ifeq ($(shell whoami),alba4k) # idk don't ask why this is here - idk clang gives useful extra info
	CC := clang
endif

ifeq ($(OS),Darwin)
	OBJ := info.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o
	INCLUDE := -framework Foundation -framework IOKit
endif

build/$(TARGET): $(OBJ)
	mkdir -p build ~/.config
	ls ~/.config/$(TARGET).conf >/dev/null 2>/dev/null || cp $(TARGET).conf ~/.config/$(TARGET).conf
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
