.PHONY: albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch
SRC1 := src/main.c
SRC2 := src/info.c
OBJ := info.o main.o
INCLUDE := -I src

build/albafetch: $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ)

main.o: $(SRC1) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC1)

info.o: $(SRC2) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC2)

test: test.c
	$(CC) -o test test.c
	./test

install: build/$(TARGET) $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ)
	cp build/$(TARGET) /usr/bin/$(TARGET)

uninstall:
	rm /usr/bin/$(TARGET)

run: $(OBJ)
	mkdir -p build
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ) && build/$(TARGET)

clean:
	rm -r build/* test *.o
