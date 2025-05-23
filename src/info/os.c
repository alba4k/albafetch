#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/queue.h"
#include "../utils/wrappers.h"

// print the operating system name and architecture (uname -m)
int os(char *dest) {
    struct utsname name;
    uname(&name);

#ifdef __APPLE__
    if(_os_arch)
        snprintf(dest, DEST_SIZE, "macOS (%s)", name.machine);
    else
        safeStrncpy(dest, "macOS", DEST_SIZE);
#else
#ifdef __ANDROID__
    char version[16];
    char *args[] = {"getprop", "ro.build.version.release", NULL};
    execCmd(version, 16, args);

    if(_os_arch)
        snprintf(dest, DEST_SIZE, "Android %s%s(%s)", version, version[0] ? " " : "", name.machine);
    else
        snprintf(dest, DEST_SIZE, "Android %s", version);
#else
    FILE *fp = fopen("/etc/os-release", "r");
    if(fp == NULL) {
        fp = fopen("/usr/lib/os-release", "r");
        if(fp == NULL)
            return ERR_NO_FILE;
    }

    char buf[64];
    char *os_name = buf;
    char *end;

    readAfterSequence(fp, "PRETTY_NAME", buf, 64);
    fclose(fp);

    if(buf[0] == 0)
        return ERR_PARSING;

    if((end = strchr(os_name, '\n')) == 0)
        return ERR_PARSING + 0x10;
    *end = 0;

    // sometimes we have something like `ID="distro"`. yeah its stupid
    if(os_name[0] == '"' || os_name[0] == '\'')
        ++os_name;

    if((end = strchr(os_name, '"')) == 0)
        end = strchr(os_name, '\'');
    if(end)
        *end = 0;

    if(_os_arch)
        snprintf(dest, DEST_SIZE, "%s (%s)", os_name, name.machine);
    else
        safeStrncpy(dest, os_name, DEST_SIZE);
#endif // __ANDROID__
#endif // __APPLE__

    return RET_OK;
}
