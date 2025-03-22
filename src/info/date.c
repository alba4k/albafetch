#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <time.h>
#include <stdio.h>

// get the current date and time
int date(char *dest) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(dest, DEST_SIZE, config.date_format, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return RET_OK;
}
