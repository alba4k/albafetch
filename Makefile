.PHONY: compile

INSTALLPATH := /usr/local/bin
CONFIGPATH := /etc/xdg

INSTALLFLAGS := -Dm755
CONFIGFLAGS := -Dm644

OS := $(shell uname -o 2> /dev/null)

ifeq ($(OS),Android)
	INSTALLPATH := $(PREFIX)/bin
	CONFIGPATH := $(PREFIX)/etc
endif

ifeq ($(OS),Darwin)
	INSTALLFLAGS := -m755
	CONFIGFLAGS := -m644
	CONFIGPATH := ~/.config/
endif

all: compile

build:
	meson setup build

clean:
	meson setup build --wipe

compile: build
	meson compile -C build

run: compile
	build/albafetch

debug: compile
	build/debug --no-pip

deb: compile
	cd debian; \
	./makedeb.sh

install: build/albafetch
	mkdir -p $(INSTALLPATH) $(CONFIGPATH)

	install $(INSTALLFLAGS) build/albafetch $(INSTALLPATH)/albafetch

	install $(CONFIGFLAGS) albafetch.conf $(CONFIGPATH)/albafetch.conf

uninstall:
	rm $(INSTALLPATH)/albafetch

	rm $(CONFIGPATH)/albafetch.conf
