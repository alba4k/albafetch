#pragma once

#include "../info/info.h"

#include <stddef.h>

// element of a module linked list
typedef struct SModule {
    char *id;             // module identifier
    char *label;          // module label
    SInfo *(*func)(void); // function to run
    struct SModule *next; // next module
} SModule;

SModule *addModule(SModule *array, char *id);

void destroyArray(SModule *array);
