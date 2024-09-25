#include "config.h"

#include <stdbool.h>
#include <stddef.h>

// checks if the given character is between two " "
bool is_in_string(const char *str, const char *place) {
    int count = 0;

    char *ptr = (char*)str;
    while((ptr = strchr(ptr, '"')) && ptr < (char*)place) {
        ++count;
        ptr += 1;
    }

    if(count % 2 == 1)
        return true;

    return false;
}

// skip every whitespace, return the first full character
char *skip_whites(char *ptr) {
    if(ptr == NULL)
        return NULL;
    if(ptr[0] == 0)
        return NULL;

    while(*ptr) {
        if(*ptr != ' ' && *ptr != '\n' && *ptr != '\t')
            return ptr;
        ++ptr;
    }

    return NULL;
}

// skip every full character, return the first whitespace
char *skip_full(char *ptr) {
    if(ptr == NULL)
        return NULL;

    while(*ptr) {
        if(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
            return ptr;
        ++ptr;
    }

    return NULL;
}
