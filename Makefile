.PHONY: build/albafetch

OS := $(shell uname -o 2> /dev/null)

INSTALLPATH := /usr/local/bin
CONFIGPATH := /etc/xdg
PKGNAME := albafetch

INSTALLFLAGS := -Dm755
CONFIGFLAGS := -Dm644

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	CONFIGPATH := $(PREFIX)/etc
endif

ifeq ($(OS),Darwin)
	INSTALLFLAGS := -m755
	CONFIGFLAGS := -m644
	CONFIGPATH := ~/.config/

	MACOS_INFOS_H := src/macos_infos.h
	BSDWRAP_H = src/bsdwrap.h
endif

all: build/albafetch build/debug

run: build/albafetch
	build/albafetch

debug: build/debug
	build/debug --no-pip

install: build/albafetch
	mkdir -p $(INSTALLPATH) $(CONFIGPATH)

	install $(INSTALLFLAGS) build/albafetch $(INSTALLPATH)/albafetch

	install $(CONFIGFLAGS) albafetch.conf $(CONFIGPATH)/albafetch.conf

uninstall:
	rm $(INSTALLPATH)/albafetch

	rm $(CONFIGPATH)/albafetch.conf

clean:
	meson setup build --wipe

build/albafetch:
	meson compile -C build

build/debug: 
	meson compile -C build