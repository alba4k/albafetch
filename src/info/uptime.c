#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include "../macos/bsdwrap.h"
#else
#include <sys/sysinfo.h>
#endif // __APPLE__

#include "info.h"
#include "../utils/wrappers.h"

// print the current uptime
int uptime(char *dest) {
#ifdef __APPLE__
    struct timeval boottime;
    int error;
    long uptime;
    error = sysctlWrap(&boottime, sizeof(boottime), CTL_KERN, KERN_BOOTTIME);

    if(error < 0)
        return ERR_NO_INFO;

    time_t boot_seconds = boottime.tv_sec;
    time_t current_seconds = time(NULL);

    uptime = (long)difftime(current_seconds, boot_seconds);
#else
    struct sysinfo info;
    if(sysinfo(&info))
        return ERR_NO_INFO;

    const long uptime = info.uptime;
#endif // __APPLE__

    long days = uptime / 86400;
    long hours = (uptime / 3600) - (days * 24);
    long mins = (uptime / 60) - (days * 1440) - (hours * 60);

    char result[DEST_SIZE] = "";
    char str[24] = "";

    if(days) {
        snprintf(str, 24, "%ldd%c", days, hours || mins ? ' ' : 0); // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(hours) {
        snprintf(str, 24, "%ldh%c", hours, mins ? ' ' : 0); // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(mins) {
        snprintf(str, 24, "%ldm", mins); // print the number of minutes passed if more than 0
        strcat(result, str);
    } else if(uptime < 60) {
        snprintf(str, 24, "%lds", uptime); // print the number of seconds passed if less than 60
        strcat(result, str);
    }

    safeStrncpy(dest, result, DEST_SIZE);

    return RET_OK;
}
