.PHONY: build

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
BUILDDIR := build
TARGET := albafetch
SRC := src/albafetch.c src/info.c
OBJ := albafetch.o info.o
INCLUDE := -I src

build:
	$(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(BUILDDIR)/$(TARGET) $(INCLUDE) $(OBJ)

time:
	time $(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(BUILDDIR)/$(TARGET) $(INCLUDE) $(OBJ)

install:
	cp $(BUILDDIR)/$(TARGET) usr/bin/$(TARGET)

uninstall:
	rm usr/bin/$(TARGET)

run:
	./build/$(TARGET)

clean:
	rm $(TARGET) test *.o
	
