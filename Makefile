.PHONY: build

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch
SRC := src/main.c src/info.c
OBJ := main.o info.o
INCLUDE := -I src

build:
	$(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(TARGET) $(INCLUDE) $(OBJ)

test:
  $(CC) -o test test.c

time:
	time $(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(TARGET) $(INCLUDE) $(OBJ)

install:
	cp $(TARGET) usr/bin/$(TARGET)

uninstall:
	rm usr/bin/$(TARGET)

run:
	$(TARGET)

clean:
	rm $(TARGET) test *.o