#include "info.h"
#include "../queue.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>
#include <sys/utsname.h>

#ifdef __ANDROID__
#include <unistd.h>
#include <sys/wait.h>
#endif // __ANDROID__

// print the operating system name and architecture (uname -m)
int os(char *dest) {
    struct utsname name;
    uname(&name);

    #ifdef __APPLE__
        if(os_arch)
            snprintf(dest, 256, "macOS (%s)", name.machine);
        else
            strncpy(dest, "macOS", 255);
    #else
    #ifdef __ANDROID__
        char version[16];
        char *args[] = {"getprop", "ro.build.version.release", NULL},
        exec_cmd(version, 16, args);

        if(os_arch)
            snprintf(dest, 256, "Android %s%s(%s)", version, version[0] ? " " : "", name.machine);
        else
            snprintf(dest, 256, "Android %s", version);

#else
        FILE *fp = fopen("/etc/os-release", "r");
        if(fp == NULL) {
            fp = fopen("/usr/lib/os-release", "r");
            if(fp == NULL)
                return 1;
        }

        char buf[64];
        char *os_name = buf;
        char *end;

        read_after_sequence(fp, "PRETTY_NAME", buf, 64);
        fclose(fp);

        if(buf[0] == 0)
            return 1;

        if((end = strchr(os_name, '\n')) == 0)
            return 1;
        *end = 0;

        // sometimes we have something like `ID="distro"`. yeah its stupid
        if(os_name[0] == '"' || os_name[0] == '\'')
            ++os_name;

        if((end = strchr(os_name, '"')) == 0)
            end = strchr(os_name, '\'');
        if(end)
            *end = 0;

        if(os_arch)
            snprintf(dest, 256, "%s (%s)", os_name, name.machine);
        else
            strncpy(dest, os_name, 255);
    #endif // __ANDROID__
    #endif // __APPLE__

    return 0;
}
