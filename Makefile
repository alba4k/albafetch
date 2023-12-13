.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

KERNEL := $(shell uname -s 2> /dev/null)
OS := $(shell uname -o 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

SRC_INFO := src/info/bios.c src/info/colors.c src/info/cpu.c src/info/date.c\
			src/info/desktop.c src/info/gpu.c src/info/gtk_theme.c\
			src/info/host.c src/info/hostname.c src/info/kernel.c\
			src/info/light_colors.c src/info/local_ip.c\
			src/info/login_shell.c src/info/memory.c src/info/os.c\
			src/info/packages.c src/info/public_ip.c src/info/pwd.c\
			src/info/shell.c src/info/term.c src/info/uptime.c src/info/user.c
OBJ_INFO := bios.o colors.o cpu.o date.o\
			desktop.o gpu.o gtk_theme.o\
			host.o hostname.o kernel.o\
			light_colors.o local_ip.o\
			login_shell.o memory.o os.o\
			packages.o public_ip.o pwd.o\
			shell.o term.o uptime.o user.o

ifeq ($(KERNEL),Linux)
	OBJ := main.o queue.o utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/utils.c $(SRC_INFO)
	INSTALLPATH := /usr/local/bin
	INSTALL_FLAGS := -Dm 755
	INCLUDE := -l curl -l pci
endif

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	INCLUDE := -l curl
endif

ifeq ($(KERNEL),Darwin)
	OBJ := os.o main.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c $(SRC_INFO)
	INSTALLPATH := /usr/local/bin
	INCLUDE := -framework Foundation -framework IOKit -l curl
endif

all: build/$(TARGET) build/debug

build/$(TARGET): $(OBJ) infos
	mkdir -p build/
	$(CC) -o build/$(TARGET) $(OBJ) $(OBJ_INFO) $(INCLUDE) $(CFLAGS)

build/debug: $(SRC_DEBUG)
	mkdir -p build/
	$(CC) $(SRC_DEBUG) $(CFLAGS) $(INCLUDE) -o build/debug

main.o: src/main.c src/logos.h src/info/info.h src/utils.h src/queue.h
	$(CC) -c src/main.c $(CFLAGS)

infos: $(SRC_INFO) src/utils.h src/queue.h src/macos_infos.h src/bsdwrap.h
	$(CC) -c $(SRC_INFO) $(CFLAGS)

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
