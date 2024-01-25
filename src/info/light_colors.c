#include "info.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>

// show the terminal color configuration (light version)
int light_colors(char *dest) {
    if(config.col_block_len > 16)
        return 1;

    memset(dest, 0, 256);
    for(int i = 0; i < 8; ++i) {
        sprintf(dest+(6+config.col_block_len)*i, "\033[10%dm", i);
        for(int j = 0; j < config.col_block_len; ++j)
            strcat(dest, " ");
    }

    strcat(dest, "\033[0m");

    return 0;
}
