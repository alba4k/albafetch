.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

KERNEL := $(shell uname -s 2> /dev/null)
OS := $(shell uname -o 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

ifeq ($(OS),GNU/Linux)
	OBJ := info.o main.o queue.o utils.o
	SRC := src/main.c src/info.c src/queue.c src/utils.c
	INSTALLPATH := /usr/bin
	INCLUDE := -l curl -l pci
endif

ifeq ($(OS),Android)
	OBJ := info.o main.o queue.o utils.o
	SRC := src/main.c src/info.c src/queue.c src/utils.c
	INSTALLPATH := $(PREFIX)/bin
	INCLUDE := -l curl
endif

ifeq ($(KERNEL),Darwin)
	INSTALLPATH := /usr/local/bin
	SRC := src/main.c src/info.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c
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

debug:
	mkdir -p build/
	$(CC) $(SRC) $(CFLAGS) $(INCLUDE) -D_DEBUG -o build/debug

	build/debug

install: build/$(TARGET)
	cp -f build/$(TARGET) $(DESTDIR)$(INSTALLPATH)/$(TARGET)

uninstall:
	rm $(INSTALLPATH)/$(TARGET)

clean:
	-rm -rf build *.o
