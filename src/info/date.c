#include "info.h"
#include "../config/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// get the current date and time
SInfo *date(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    const size_t SIZE = 128;
    res->string = malloc(SIZE);
    res->retval = RET_OK;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    snprintf(res->string, SIZE, config.date_format, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return res;
}
