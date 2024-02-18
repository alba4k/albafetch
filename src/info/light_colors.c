#include "info.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>

// show the terminal color configuration
int light_colors(char *dest) {
    memset(dest, 0, 256);
    
    for(int i = 0; i < 8; ++i)
        sprintf(dest+strlen(dest), "\033[%s%dm%s", col_foreground ? "9" : "10", i, config.col_block_str);

    strcat(dest, "\033[0m");

    return 0;
}
