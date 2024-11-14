#include "config.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// TODO: rewrite config parsing from scratch

/*

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

*/

// remove comments, aka from start to the end of the line
void uncomment(char *str, const char start) {
    char *ptr = str, *ptr2;
    while((ptr = strchr(ptr, start))) {
        // when it is between two " (aka the number of " before it is odd)
        int counter = 0;
        ptr2 = str;
        while((ptr2 = strchr(ptr2, '"')) && ptr2 < ptr) {
            ++ptr2;
            ++counter;
        }
        if(counter&1) {
            ++ptr;
            continue;
        }

        ptr2 = strchr(ptr, '\n');
        
        if(ptr2 == NULL) {
            *ptr = 0;
            break;
        }

        memmove(ptr, ptr2+1, strlen(ptr2));
    }
}

// check every '\' in str and unescape "\\" "\n" "\e" "\033"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        switch(str[1]) {
            case 'e':
                memmove(str, str+1, strlen(str));
                *str = '\033';
                break;
            case '0':
                if(str[2] == '3' && str[3] == '3') {
                    memmove(str, str+3, strlen(str+2));
                    *str = '\033';
                }
                break;
            case 'n':
                memmove(str, str+1, strlen(str));
                *str = '\n';
                break;
            default:    // takes care of "\\" and any other sort of "\X"
                memmove(str, str+1, strlen(str));
                ++str;
                break;
        }
    }
}
