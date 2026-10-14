#include "config.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../utils/return.h"

// TODO: rewrite config parsing from scratch

// remove comments, aka from start to the end of the line
void uncomment(char *str, const char start) {
    char *ptr = str;
    int counter = 0;

    while(ptr[0] != 0) {
        if(ptr[0] == '\"' && (ptr == str || ptr[-1] != '\\')) {
            ++counter;
            ++ptr;
            continue;
        }

        if(ptr[0] == start && counter % 2 == 0) {
            while(ptr[0] != '\n' && ptr[0] != '\0') {
                ptr[0] = ' '; // blank out from the comment start to the end of line
                ++ptr;
            }
        }
        else
            ++ptr;
    }
}

// check every '\' in str and unescape "\\" "\n" "\e" "\033"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        switch(str[1]) {
            case 'e':
                memmove(str, str + 1, strlen(str));
                *str = '\033';
                break;
            case '0':
                if(str[2] == '3' && str[3] == '3') {
                    memmove(str, str + 3, strlen(str + 2));
                    *str = '\033';
                }
                break;
            case 'n':
                memmove(str, str + 1, strlen(str));
                *str = '\n';
                break;
            default: // takes care of "\\" and any other sort of "\X"
                memmove(str, str + 1, strlen(str));
                ++str;
                break;
        }
    }
}
