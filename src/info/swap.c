#include "../config/config.h"
#include "info.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <sys/sysinfo.h>
#endif // __APPLE__

// get used and total swap
SInfo *swap(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifdef __APPLE__
    // don't really know how to do it and am too lazy to research it, so...
    res->retval = ERR_UNSUPPORTED;
#else
    struct sysinfo info;
    if(sysinfo(&info)) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    unsigned long totalswap = info.totalswap / 1024;
    unsigned long freeswap = info.freeswap / 1024;

    if(totalswap * freeswap == 0) { // one or the other
        res->retval = ERR_NO_INFO;
        return res;
    }

    unsigned long usedswap = totalswap - freeswap;

    res->string = malloc(64);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
    snprintf(res->string, 64, "%lu MiB / %lu MiB", usedswap / 1024, totalswap / 1024);

    if(config.swap_perc) {
        const size_t len = 64 - strlen(res->string);
        char perc[len];

        snprintf(perc, len, " (%lu%%)", (usedswap * 100) / totalswap);
        strcat(res->string, perc);
    }
#endif

    return res;
}
