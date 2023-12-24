.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast
TARGET := albafetch

KERNEL := $(shell uname -s 2> /dev/null)
OS := $(shell uname -o 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

INSTALLPATH := /usr/local/bin

OBJ_INFO := bios.o colors.o cpu.o date.o\
			desktop.o gpu.o gtk_theme.o\
			host.o hostname.o kernel.o\
			light_colors.o local_ip.o\
			login_shell.o memory.o os.o\
			packages.o public_ip.o pwd.o\
			shell.o term.o uptime.o user.o

ifeq ($(KERNEL),Linux)
	OBJ := main.o queue.o utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/utils.c
	OBJ_DEBUG := debug.o queue.o utils.o
	INSTALL_FLAGS := -Dm 755
	INCLUDE := -l pci
endif

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	INCLUDE := 
endif

ifeq ($(KERNEL),Darwin)
	OBJ := main.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c
	OBJ_DEBUG := debug.o queue.o macos_infos.o bsdwrap.o macos_gpu_string.o utils.o
	INCLUDE := -framework Foundation -framework IOKit

	MACOS_INFOS_H := src/macos_infos.h
	BSDWRAP_H = src/bsdwrap.h
endif

all: build/$(TARGET) build/debug

run: build/$(TARGET)
	build/$(TARGET)

debug: build/debug
	build/debug --no-pip

install: build/$(TARGET)
	@install $(INSTALL_FLAGS) build/$(TARGET) $(INSTALLPATH)/$(TARGET) || \
	bash -c 'echo -e "\e[31m\e[1mERROR\e[0m: Running without root proviliges?"'

uninstall:
	rm $(INSTALLPATH)/$(TARGET)

clean:
	-rm -rf *.o

build/$(TARGET): $(OBJ) $(OBJ_INFO)
	mkdir -p build/
	$(CC) -o build/$(TARGET) $(OBJ) $(OBJ_INFO) $(INCLUDE) $(CFLAGS)

build/debug: $(OBJ_DEBUG) $(OBJ_INFO)
	mkdir -p build/
	$(CC) $(OBJ_DEBUG) $(OBJ_INFO) $(CFLAGS) $(INCLUDE) -o build/debug

main.o: src/main.c src/logos.h src/info/info.h src/utils.h src/queue.h
	$(CC) -c src/main.c $(CFLAGS)

utils.o: src/utils.c
	$(CC) -c src/utils.c $(CFLAGS)

bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c $(CFLAGS)

macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c $(CFLAGS)

queue.o: src/queue.c src/logos.h
	$(CC) -c src/queue.c $(CFLAGS)

macos_gpu_string.o: src/macos_gpu_string.m
	$(CC) -c src/macos_gpu_string.m $(CFLAGS)

bios.o: src/info/bios.c src/info/info.h
	$(CC) -c src/info/bios.c $(CFLAGS)

colors.o: src/info/colors.c src/info/info.h src/utils.h
	$(CC) -c src/info/colors.c $(CFLAGS)

cpu.o: src/info/cpu.c src/info/info.h src/utils.h
	$(CC) -c src/info/cpu.c $(CFLAGS)

date.o: src/info/date.c src/info/info.h src/utils.h
	$(CC) -c src/info/date.c $(CFLAGS)

debug.o: src/debug.c src/info/info.h src/utils.h
	$(CC) -c src/debug.c $(CFLAGS)

desktop.o: src/info/desktop.c src/info/info.h src/utils.h
	$(CC) -c src/info/desktop.c $(CFLAGS)

gpu.o: src/info/gpu.c src/info/info.h src/utils.h $(MACOS_INFOS_H)
	$(CC) -c src/info/gpu.c $(CFLAGS)

gtk_theme.o: src/info/gtk_theme.c src/info/info.h
	$(CC) -c src/info/gtk_theme.c $(CFLAGS)

host.o: src/info/host.c src/info/info.h
	$(CC) -c src/info/host.c $(CFLAGS)

hostname.o: src/info/hostname.c src/info/info.h
	$(CC) -c src/info/hostname.c $(CFLAGS)

kernel.o: src/info/kernel.c src/info/info.h src/utils.h
	$(CC) -c src/info/kernel.c $(CFLAGS)

light_colors.o: src/info/light_colors.c src/info/info.h src/utils.h
	$(CC) -c src/info/light_colors.c $(CFLAGS)

local_ip.o: src/info/local_ip.c src/info/info.h src/utils.h
	$(CC) -c src/info/local_ip.c $(CFLAGS)

login_shell.o: src/info/login_shell.c src/info/info.h src/utils.h
	$(CC) -c src/info/login_shell.c $(CFLAGS)

memory.o: src/info/memory.c src/info/info.h src/utils.h src/queue.h $(MACOS_INFOS_H)
	$(CC) -c src/info/memory.c $(CFLAGS)

os.o: src/info/os.c src/info/info.h src/utils.h src/queue.h
	$(CC) -c src/info/os.c $(CFLAGS)

packages.o: src/info/packages.c src/info/info.h src/utils.h
	$(CC) -c src/info/packages.c $(CFLAGS)

public_ip.o: src/info/public_ip.c src/info/info.h
	$(CC) -c src/info/public_ip.c $(CFLAGS)

pwd.o: src/info/pwd.c src/info/info.h src/utils.h
	$(CC) -c src/info/pwd.c $(CFLAGS)

shell.o: src/info/shell.c src/info/info.h src/utils.h
	$(CC) -c src/info/shell.c $(CFLAGS)

term.o: src/info/term.c src/info/info.h src/utils.h
	$(CC) -c src/info/term.c $(CFLAGS)

uptime.o: src/info/uptime.c src/info/info.h $(BSDWRAP_H)
	$(CC) -c src/info/uptime.c $(CFLAGS)

user.o: src/info/user.c src/info/info.h
	$(CC) -c src/info/user.c $(CFLAGS)
