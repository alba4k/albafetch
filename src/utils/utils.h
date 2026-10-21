#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../info/info.h"

// element of a module linked list
struct SModule {
    char *id;             // module identifier
    char *label;          // module label
    SInfo *(*func)(void); // function to run
    struct SModule *next; // next module
};

void *fileToLogo(char *file);

void addModule(struct SModule *array, char *id);

void destroyArray(struct SModule *array);

void getLogoLine(char *dest, unsigned *line);

void printLine(char *line, const size_t maxlen);
