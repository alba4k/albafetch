.PHONY: build/albafetch

CC := gcc
CFLAGS := -Wall -Wextra -Ofast

KERNEL := $(shell uname -s 2> /dev/null)
OS := $(shell uname -o 2> /dev/null)
PACMAN := $(shell ls /bin/pacman 2> /dev/null)

INSTALLPATH := /usr/local/bin
DATAPATH := /usr/local/share
CONFIGPATH := /etc/xdg
PKGNAME := albafetch

OBJ_INFO := obj/bios.o obj/colors.o obj/cpu.o obj/date.o\
			obj/desktop.o obj/gpu.o obj/gtk_theme.o obj/icon_theme.o\
			obj/cursor_theme.o obj/host.o obj/hostname.o obj/kernel.o\
			obj/light_colors.o obj/local_ip.o\
			obj/login_shell.o obj/memory.o obj/os.o\
			obj/packages.o obj/public_ip.o obj/pwd.o\
			obj/shell.o obj/term.o obj/uptime.o obj/user.o

ifeq ($(KERNEL),Linux)
	OBJ := obj/main.o obj/queue.o obj/utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/utils.c
	OBJ_DEBUG := obj/debug.o obj/queue.o obj/utils.o
	INSTALL_FLAGS := -Dm 755
	INCLUDE := -l pci
endif

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	INCLUDE := 
endif

ifeq ($(KERNEL),Darwin)
	OBJ := obj/main.o obj/macos_infos.o obj/bsdwrap.o obj/macos_gpu_string.o obj/utils.o
	SRC_DEBUG := src/debug.c src/queue.c src/macos_infos.c src/bsdwrap.c src/macos_gpu_string.m src/utils.c
	OBJ_DEBUG := obj/debug.o obj/queue.o obj/macos_infos.o obj/bsdwrap.o obj/macos_gpu_string.o obj/utils.o
	INCLUDE := -framework Foundation -framework IOKit

	MACOS_INFOS_H := src/macos_infos.h
	BSDWRAP_H = src/bsdwrap.h
endif

all: build/albafetch build/debug

run: build/albafetch
	build/albafetch

debug: build/debug
	build/debug --no-pip

install: build/albafetch
	mkdir -p $(INSTALLPATH) $(DATAPATH)/licenses/$(PKGNAME) $(DATAPATH)/doc/$(PKGNAME) $(CONFIGPATH)

	install -Dm755 build/albafetch $(INSTALLPATH)/albafetch

	install -Dm644 LICENSE $(DATAPATH)/licenses/$(PKGNAME)/LICENSE
	install -Dm644 README.md $(DATAPATH)/doc/$(PKGNAME)/README.md
	install -Dm644 MANUAL.md $(DATAPATH)/doc/$(PKGNAME)/MANUAL.md

	install -Dm644 albafetch.conf $(CONFIGPATH)/albafetch.conf

uninstall:
	rm $(INSTALLPATH)/albafetch

	rm $(DATAPATH)/licenses/$(PKGNAME)/LICENSE
	rm $(DATAPATH)/doc/$(PKGNAME)/README.md
	rm $(DATAPATH)/doc/$(PKGNAME)/MANUAL.md

	rm $(CONFIGPATH)/albafetch.conf

clean:
	rm -rf obj/*.o

build/albafetch: $(OBJ) $(OBJ_INFO)
	mkdir -p build/
	$(CC) -o build/albafetch $(OBJ) $(OBJ_INFO) $(INCLUDE) $(CFLAGS)

build/debug: $(OBJ_DEBUG) $(OBJ_INFO)
	mkdir -p build/
	$(CC) $(OBJ_DEBUG) $(OBJ_INFO) $(CFLAGS) $(INCLUDE) -o build/debug

obj/bios.o: src/info/bios.c src/info/info.h
	$(CC) -c src/info/bios.c $(CFLAGS) -o obj/bios.o

obj/bsdwrap.o: src/bsdwrap.c
	$(CC) -c src/bsdwrap.c $(CFLAGS) -o obj/bsdwrap.o

obj/colors.o: src/info/colors.c src/info/info.h src/utils.h
	$(CC) -c src/info/colors.c $(CFLAGS) -o obj/colors.o

obj/cpu.o: src/info/cpu.c src/info/info.h src/utils.h
	$(CC) -c src/info/cpu.c $(CFLAGS) -o obj/cpu.o

obj/cursor_theme.o: src/info/cursor_theme.c src/info/info.h
	$(CC) -c src/info/cursor_theme.c $(CFLAGS) -o obj/cursor_theme.o

obj/date.o: src/info/date.c src/info/info.h src/utils.h
	$(CC) -c src/info/date.c $(CFLAGS) -o obj/date.o

obj/debug.o: src/debug.c src/info/info.h src/utils.h
	$(CC) -c src/debug.c $(CFLAGS) -o obj/debug.o

obj/desktop.o: src/info/desktop.c src/info/info.h src/utils.h
	$(CC) -c src/info/desktop.c $(CFLAGS) -o obj/desktop.o

obj/gpu.o: src/info/gpu.c src/info/info.h src/utils.h $(MACOS_INFOS_H)
	$(CC) -c src/info/gpu.c $(CFLAGS) -o obj/gpu.o

obj/gtk_theme.o: src/info/gtk_theme.c src/info/info.h
	$(CC) -c src/info/gtk_theme.c $(CFLAGS) -o obj/gtk_theme.o

obj/icon_theme.o: src/info/icon_theme.c src/info/info.h
	$(CC) -c src/info/icon_theme.c $(CFLAGS) -o obj/icon_theme.o

obj/host.o: src/info/host.c src/info/info.h
	$(CC) -c src/info/host.c $(CFLAGS) -o obj/host.o

obj/hostname.o: src/info/hostname.c src/info/info.h
	$(CC) -c src/info/hostname.c $(CFLAGS) -o obj/hostname.o

obj/kernel.o: src/info/kernel.c src/info/info.h src/utils.h
	$(CC) -c src/info/kernel.c $(CFLAGS) -o obj/kernel.o

obj/light_colors.o: src/info/light_colors.c src/info/info.h src/utils.h
	$(CC) -c src/info/light_colors.c $(CFLAGS) -o obj/light_colors.o

obj/local_ip.o: src/info/local_ip.c src/info/info.h src/utils.h
	$(CC) -c src/info/local_ip.c $(CFLAGS) -o obj/local_ip.o

obj/login_shell.o: src/info/login_shell.c src/info/info.h src/utils.h
	$(CC) -c src/info/login_shell.c $(CFLAGS) -o obj/login_shell.o

obj/macos_gpu_string.o: src/macos_gpu_string.m
	$(CC) -c src/macos_gpu_string.m $(CFLAGS) -o obj/macos_gpu_string.o

obj/macos_infos.o: src/macos_infos.c
	$(CC) -c src/macos_infos.c $(CFLAGS) -o obj/macos_infos.o

obj/main.o: src/main.c src/logos.h src/info/info.h src/utils.h src/queue.h
	$(CC) -c src/main.c $(CFLAGS) -o obj/main.o

obj/memory.o: src/info/memory.c src/info/info.h src/utils.h src/queue.h $(MACOS_INFOS_H)
	$(CC) -c src/info/memory.c $(CFLAGS) -o obj/memory.o

obj/os.o: src/info/os.c src/info/info.h src/utils.h src/queue.h
	$(CC) -c src/info/os.c $(CFLAGS) -o obj/os.o

obj/packages.o: src/info/packages.c src/info/info.h src/utils.h
	$(CC) -c src/info/packages.c $(CFLAGS) -o obj/packages.o

obj/public_ip.o: src/info/public_ip.c src/info/info.h
	$(CC) -c src/info/public_ip.c $(CFLAGS) -o obj/public_ip.o

obj/pwd.o: src/info/pwd.c src/info/info.h src/utils.h
	$(CC) -c src/info/pwd.c $(CFLAGS) -o obj/pwd.o

obj/queue.o: src/queue.c src/logos.h
	$(CC) -c src/queue.c $(CFLAGS) -o obj/queue.o

obj/shell.o: src/info/shell.c src/info/info.h src/utils.h
	$(CC) -c src/info/shell.c $(CFLAGS) -o obj/shell.o

obj/term.o: src/info/term.c src/info/info.h src/utils.h
	$(CC) -c src/info/term.c $(CFLAGS) -o obj/term.o

obj/uptime.o: src/info/uptime.c src/info/info.h $(BSDWRAP_H)
	$(CC) -c src/info/uptime.c $(CFLAGS) -o obj/uptime.o

obj/user.o: src/info/user.c src/info/info.h
	$(CC) -c src/info/user.c $(CFLAGS) -o obj/user.o

obj/utils.o: src/utils.c
	$(CC) -c src/utils.c $(CFLAGS) -o obj/utils.o
