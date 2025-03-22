#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// element of a module linked list
struct SModule {
    char *id;             // module identifier
    char *label;          // module label
    int (*func)(char *);  // function to run
    struct SModule *next; // next module
};

void *fileToLogo(char *file);

void addModule(struct SModule *array, char *id);

void destroyArray(struct SModule *array);

void getLogoLine(char *dest, unsigned *line);

void printLine(char *line, const size_t maxlen);

void unescape(char *str);
