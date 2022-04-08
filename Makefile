.PHONY: albafetch

SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
TARGET := albafetch
SRC1 := src/main.c
SRC2 := src/info.c
OBJ := main.o info.o
INCLUDE := -I src

albafetch: $(OBJ)
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ)

main.o: $(SRC1) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC1)

info.o: $(SRC2) src/config.h src/vars.h src/info.h
	$(CC) -c $(SRC2)

test: test.c
	$(CC) -o test test.c
	./test

install: build/$(TARGET)
	cp build/$(TARGET) /usr/bin/$(TARGET)

uninstall:
	rm /usr/bin/$(TARGET)

run: $(OBJ)
	$(CC) -o build/$(TARGET) $(INCLUDE) $(OBJ)
	build/$(TARGET)

clean:
	rm $(TARGET) test *.o
