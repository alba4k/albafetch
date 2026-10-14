#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "../macos/bsdwrap.h"
#else
#include <sys/sysinfo.h>
#endif // __APPLE__

#include "info.h"
#include "../utils/wrappers.h"

// print the current uptime
SInfo *uptime(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifdef __APPLE__
    struct timeval boottime;
    int error;
    long uptime;
    error = sysctlWrap(&boottime, sizeof(boottime), CTL_KERN, KERN_BOOTTIME);

    if(error < 0) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    time_t boot_seconds = boottime.tv_sec;
    time_t current_seconds = time(NULL);

    uptime = (long)difftime(current_seconds, boot_seconds);
#else
    struct sysinfo info;
    if(sysinfo(&info)) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    const long uptime = info.uptime;
#endif // __APPLE__

    long days = uptime / 86400;
    long hours = (uptime / 3600) - (days * 24);
    long mins = (uptime / 60) - (days * 1440) - (hours * 60);

    res->string = malloc(96);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
    res->string[0] = 0;
    char buf[24] = "";

    if(days) {
        snprintf(buf, 24, "%ldd%c", days, hours || mins ? ' ' : 0); // print the number of days passed if more than 0
        strcat(res->string, buf);
    }
    if(hours) {
        snprintf(buf, 24, "%ldh%c", hours, mins ? ' ' : 0); // print the number of days passed if more than 0
        strcat(res->string, buf);
    }
    if(mins) {
        snprintf(buf, 24, "%ldm", mins); // print the number of minutes passed if more than 0
        strcat(res->string, buf);
    }
    else if(uptime < 60) {
        snprintf(buf, 24, "%lds", uptime); // print the number of seconds passed if less than 60
        strcat(res->string, buf);
    }

    return res;
}
