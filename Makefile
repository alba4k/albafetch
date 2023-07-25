.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

KERNEL := $(shell uname -s 2> /dev/null)
OS := $(shell uname -o 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

ifeq ($(KERNEL),Linux)
	OBJ := info.o main.o queue.o utils.o
	SRC := src/main.c src/info.c src/queue.c src/utils.c
	SRC_DEBUG := src/debug.c src/info.c src/queue.c src/utils.c
	INSTALLPATH := /usr/local/bin
	INSTALL_FLAGS := -Dm 755
	INCLUDE := -l curl -l pci
endif

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	INCLUDE := -l curl
endif

ifeq ($(KERNEL),Darwin)
	OBJ := info.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	SRC := src/main.c src/info.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c
	SRC_DEBUG := src/debug.c src/info.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c
	INSTALLPATH := /usr/local/bin
	INCLUDE := -framework Foundation -framework IOKit -l curl
endif

all: build/$(TARGET) build/debug

build/$(TARGET): $(OBJ)
	mkdir -p build/
	$(CC) -o build/$(TARGET) $(OBJ) $(INCLUDE) $(CFLAGS)

build/debug: $(SRC_DEBUG)
	mkdir -p build/
	$(CC) $(SRC_DEBUG) $(CFLAGS) $(INCLUDE) -o build/debug

main.o: src/main.c src/logos.h src/info.h src/utils.h src/queue.h
	$(CC) -c src/main.c $(CFLAGS)

info.o: src/info.c src/utils.h src/queue.h src/macos_infos.h src/bsdwrap.h
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

debug: build/debug
	build/debug

install: build/$(TARGET)
	@install $(INSTALL_FLAGS) build/$(TARGET) $(INSTALLPATH)/$(TARGET) || \
	bash -c 'echo -e "\e[31m\e[1mERROR\e[0m: Running without root proviliges?"'

uninstall:
	rm $(INSTALLPATH)/$(TARGET)

clean:
	-rm -rf build *.o
