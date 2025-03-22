#include "info.h"
#include "../utils/queue.h"
#include "../config/config.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include "../macos/macos_infos.h"
#else
#include <sys/sysinfo.h>
#endif // __APPLE__

// get used and total memory
int memory(char *dest) {
#ifdef __APPLE__
    bytes_t usedram = usedMemSize();
    bytes_t totalram = systemMemSize();

    if(usedram == 0 || totalram == 0) {
        return ERR_NO_INFO;
    }

    snprintf(dest, DEST_SIZE, "%llu MiB / %llu MiB", usedram / 1048576, totalram / 1048576);
#else
    struct sysinfo info;
    if(sysinfo(&info))
        return ERR_NO_INFO;

    unsigned long totalram = info.totalram / 1024;
    unsigned long freeram = info.freeram / 1024;
    // unsigned long sharedram = info.sharedram / 1024;

    FILE *fp = fopen("/proc/meminfo", "r");

    if(fp == NULL)
        return ERR_NO_FILE;

    char buf[DEST_SIZE];
    char *cachedram = buf;

    readAfterSequence(fp, "Cached:", buf, DEST_SIZE);
    fclose(fp);

    if(buf[0] == 0)
        return ERR_PARSING;
    cachedram += 2;

    char *end = strstr(cachedram, " kB");

    if(end == NULL)
        return ERR_PARSING + 0x10;

    *end = 0;

    unsigned long usedram = totalram - freeram - atol(cachedram);
    // usedram -= sharedram;

    snprintf(dest, DEST_SIZE, "%lu MiB / %lu MiB", usedram / 1024, totalram / 1024);
#endif

    if(_mem_perc) {
        const size_t len = DEST_SIZE - strlen(dest);
        char perc[len];

        snprintf(perc, len, " (%lu%%)", (usedram * 100) / totalram);
        strcat(dest, perc);
    }

    return RET_OK;
}
