#include "../config/config.h"
#include "info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// date_format is given to snprintf along with 6 ints, so it can only safely contain up to 6 "%d"s
static bool validDateFormat(const char *format) {
    int conversions = 0;

    while((format = strchr(format, '%'))) {
        ++format;

        if(*format == '%') { // literal %
            ++format;
            continue;
        }

        format += strspn(format, "-+ 0");

        size_t width = strspn(format, "0123456789");
        if(width > 3)
            return false;
        format += width;

        if(*format != 'd' || ++conversions > 6)
            return false;
        ++format;
    }

    return true;
}

// get the current date and time
SInfo *date(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    if(!validDateFormat(config.date_format)) {
        res->retval = ERR_PARSING;
        return res;
    }

    const size_t SIZE = 128;
    res->string = malloc(SIZE);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    snprintf(res->string, SIZE, config.date_format, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return res;
}
