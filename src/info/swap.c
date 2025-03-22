#include "info.h"
#include "../utils/queue.h"
#include "../config/config.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <sys/sysinfo.h>
#endif // __APPLE__

// get used and total swap
int swap(char *dest) {
#ifdef __APPLE__
    // don't really know how to do it and am too lazy to research it, so...
    (void)dest;
    return ERR_UNSUPPORTED;
#else
    struct sysinfo info;
    if(sysinfo(&info))
        return ERR_NO_INFO;

    unsigned long totalswap = info.totalswap / 1024;
    unsigned long freeswap = info.freeswap / 1024;

    if(totalswap * freeswap == 0) // one or the other
        return ERR_NO_INFO;

    unsigned long usedswap = totalswap - freeswap;

    snprintf(dest, DEST_SIZE, "%lu MiB / %lu MiB", usedswap / 1024, totalswap / 1024);

    if(_swap_perc) {
        const size_t len = DEST_SIZE - strlen(dest);
        char perc[len];

        snprintf(perc, len, " (%lu%%)", (usedswap * 100) / totalswap);
        strcat(dest, perc);
    }
#endif

    return RET_OK;
}
