#include <string.h>
#include <unistd.h>
#include <stdio.h>

#ifdef __APPLE__
#include "bsdwrap.h"
#include "macos_infos.h"
#else
#include <sys/sysinfo.h> // uptime, memory
#endif // __APPLE__

#include <sys/utsname.h> // uname
#include <pwd.h> // username

#include "info.h"
#include "queue.h"

int user(char *dest) {
    struct passwd *pw;

    unsigned uid = geteuid();
    if((int)uid == -1) {
        // couldn't get UID
        return 1;
    }

    pw = getpwuid(uid);

    strncpy(dest, pw->pw_name, LOGIN_NAME_MAX);

    return 0;
}

int hostname(char *dest) {
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    strncpy(dest, hostname, HOST_NAME_MAX);

    return 0;
}

int uptime(char *dest) {
    #ifdef __APPLE__
        struct timeval boottime;
        int error;
        long uptime;
        error = sysctl_wrap(&boottime, sizeof(boottime), CTL_KERN, KERN_BOOTTIME);

        if(error < 0)
            return 1;

        time_t boot_seconds = boottime.tv_sec;
        time_t current_seconds = time(NULL);

        uptime = (long)difftime(current_seconds, boot_seconds);
    #else
        struct sysinfo info;
        sysinfo(&info);

        const long uptime = info.uptime;
    #endif // __APPLE__

    long days = uptime/86400;
    char hours = uptime/3600 - days*24;
    char mins = uptime/60 - days*1440 - hours*60;

    char result[24] = "";
    char str[16] = "";

    if(days) {
        snprintf(str, 16, "%ldd ", days);     // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(hours) {
        snprintf(str, 16, "%dh ", hours);      // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(mins) {
        snprintf(str, 16, "%dm ", mins);       // print the number of minutes passed if more than 0
        strcat(result, str);
    }
    else if(uptime < 60) {
        snprintf(str, 16, "%lds", uptime);       // print the number of seconds passed if less than 60
        strcat(result, str);
    }

    strncpy(dest, result, 256);

    return 0;
}

int os(char *dest) {
    struct utsname name;
    uname(&name);

    #ifdef __APPLE__
        snprintf(dest, 256, "macOS %s", name.machine);
        return 0;
    #else
        FILE *fp = fopen("/etc/os-release", "r");
        if(!fp) {
            return 1;
        }

        char buf[64];
        char *os_name = buf;
        char *end;

        read_after_sequence(fp, "PRETTY_NAME", buf, 64);
        fclose(fp);

        if(!buf[0])
            return 1;

        if(os_name[0] == '"' || os_name[0] == '\'')
            ++os_name;
        
        if(!(end = strchr(os_name, '\n')))
            return 1;
        *end = 0;

        if((end = strchr(os_name, '"')))
            *end = 0;
        else if((end = strchr(os_name, '\'')))
            *end = 0;

        snprintf(dest, 256, "%s %s", os_name, name.machine);

        return 0;
    #endif // __APPLE__
}
