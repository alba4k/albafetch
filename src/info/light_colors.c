#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <stdio.h>

// show the terminal color configuration
int lightColors(char *dest) {
    memset(dest, 0, DEST_SIZE);

    for(int i = 0; i < 8; ++i)
        sprintf(dest + strlen(dest), "\033[%s%dm%s", _col_background ? "10" : "9", i, config.col_block_str);

    strcat(dest, "\033[0m");

    return RET_OK;
}
