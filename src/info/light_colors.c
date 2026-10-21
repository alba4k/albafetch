#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../config/config.h"
#include "info.h"

// show the terminal color configuration
SInfo *lightColors(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->retval = RET_OK;

    res->string = calloc((8 * (strlen(config.col_block_str) + (_col_background ? 2 : 1) + 4)) + 5, sizeof(char));
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }

    for(int i = 0; i < 8; ++i)
        sprintf(res->string + strlen(res->string), "\033[%s%dm%s", _col_background ? "10" : "9", i, config.col_block_str);

    strcat(res->string, "\033[0m");

    return res;
}
