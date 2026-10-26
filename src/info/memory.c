#include "../config/config.h"
#include "info.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include "../macos/macos_infos.h"
#endif // __APPLE__

// get used and total memory
SInfo *memory(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifdef __APPLE__
    unsigned long usedram = (unsigned long)usedMemSize();
    unsigned long totalram = (unsigned long)systemMemSize();

    if(usedram == 0 || totalram == 0) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    res->string = malloc(64);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
    snprintf(res->string, 64, "%lu / %lu MiB", usedram / 1048576, totalram / 1048576);

#else
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL) {
        res->retval = ERR_NO_FILE;
        return res;
    }

    unsigned long totalram = 0;
    unsigned long available = 0;
    int found = 0;

    char line[256];
    while(fgets(line, sizeof(line), meminfo)) {
        if(sscanf(line, "MemTotal: %lu kB", &totalram) == 1)
            found++;
        else if(sscanf(line, "MemAvailable: %lu kB", &available) == 1)
            found++;

        if(found >= 2)
            break;
    }
    fclose(meminfo);

    if(found < 2) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    unsigned long usedram = totalram - available;

    res->string = malloc(64);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
    snprintf(res->string, 64, "%lu / %lu MiB", usedram / 1024, totalram / 1024);
#endif

    if(_mem_perc) {
        const size_t len = 64 - strlen(res->string);
        char perc[len];

        snprintf(perc, len, " (%lu%%)", (usedram * 100) / totalram);
        strcat(res->string, perc);
    }

    return res;
}
