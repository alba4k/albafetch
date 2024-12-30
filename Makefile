.PHONY: all compile clean run debug deb install uninstall

OS := $(shell uname -o 2> /dev/null)
KERNEL := $(shell uname -s 2> /dev/null)

INSTALLPATH := /usr/local/bin
CONFIGPATH := /etc/xdg

INSTALLFLAGS := -Dm755
CONFIGFLAGS := -Dm644

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	CONFIGPATH := $(PREFIX)/etc/xdg
endif

ifeq ($(KERNEL),Darwin)
	INSTALLFLAGS := -m755
	CONFIGFLAGS := -m644
	INSTALLPATH := $(PREFIX)/bin
	CONFIGPATH := ~/.config/
endif

all: compile

build:
	meson setup build

clean:
	rm -rf build

compile: build
	meson compile -C build albafetch

run: compile
	build/albafetch

debug: build
	meson compile -C build debug
	build/debug --no-pip

deb: compile
	cd debian; \
	./makedeb.sh

install: compile
	mkdir -p $(INSTALLPATH) $(CONFIGPATH)

	install $(INSTALLFLAGS) build/albafetch $(INSTALLPATH)/albafetch
	install $(CONFIGFLAGS) albafetch.conf $(CONFIGPATH)/albafetch.conf

uninstall:
	rm $(INSTALLPATH)/albafetch
	rm $(CONFIGPATH)/albafetch.conf
