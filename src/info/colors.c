#include "info.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>

// show the terminal color configuration
int colors(char *dest) {
    memset(dest, 0, 256);
    for(int i = 0; i < 8; ++i) {
        sprintf(dest+(5+config.col_block_len)*i, "\033[4%dm", i);
        for(int i = 0; i < config.col_block_len; ++i)
            strcat(dest, " ");
    }

    strcat(dest, "\033[0m");

    return 0;
}
