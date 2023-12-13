#include "info.h"
#include "../utils.h"

#include <string.h>

#include <time.h>
#include <stdio.h>


// get the current date and time
int date(char *dest) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(dest, 256, config.date_format, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return 0;
}
