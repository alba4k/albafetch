#include "config.h"
#include "parsing.h"

#include <stdbool.h>
#include <stddef.h>

void uncomment(char *text) {
    #ifdef DEBUG
    debug_log("Removing comments...");
    #endif // DEBUG

    char *ptr = text;
    while((ptr = strchr(ptr, '#'))) {
        if(is_in_string(text, ptr) == true) {
            ++ptr;
            continue;
        }

        char *ptr2 = strchr(ptr, '\n');
        if(ptr2 != NULL)
            memmove(ptr, ptr2 + 1, strlen(ptr2));
        else
            *ptr = 0;

        ++ptr;
    }
}

int parse_config(const char *file) {

}
