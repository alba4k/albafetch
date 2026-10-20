#include "info.h"
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
    struct sysinfo info;
    if(sysinfo(&info)) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    unsigned long totalram = info.totalram / 1024;
    unsigned long freeram = info.freeram / 1024;
    // unsigned long sharedram = info.sharedram / 1024;

    unsigned long usedram = totalram - freeram;
    // usedram -= sharedram;

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
