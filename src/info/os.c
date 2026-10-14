#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/queue.h"
#include "../utils/wrappers.h"

// print the operating system name and architecture (uname -m)
SInfo *os(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    struct utsname name;
    uname(&name);

#ifdef __APPLE__
    if(_os_arch) {
        res->string = malloc(strlen("macOS") + 16);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, strlen("macOS") + 16, "macOS (%s)", name.machine);
    }
    else {
        res->string = strdup("macOS");
        if(res->string == NULL)
            res->retval = ERR_OOM;
    }
#else
#ifdef __ANDROID__
    char version[16];
    char *args[] = {"getprop", "ro.build.version.release", NULL};
    execCmd(version, 16, args);

    if(_os_arch) {
        res->string = malloc(32);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, 32, "Android %s%s(%s)", version, (version && version[0]) ? " " : "", name.machine);
    }
    else {
        res->string = malloc(16);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, 16, "Android %s", version);
    }
#else
    FILE *fp = fopen("/etc/os-release", "r");
    if(fp == NULL) {
        fp = fopen("/usr/lib/os-release", "r");
        if(fp == NULL) {
            res->retval = ERR_NO_FILE;
            return res;
        }
    }

    char buf[64];
    char *os_name = buf;
    char *end;

    readAfterSequence(fp, "PRETTY_NAME", buf, 64);
    fclose(fp);

    if(buf[0] == 0) {
        res->retval = ERR_PARSING;
        return res;
    }

    if((end = strchr(os_name, '\n')) == 0) {
        res->retval = ERR_PARSING + 0x10;
        return res;
    }
    *end = 0;

    // sometimes we have something like `ID="distro"`. yeah its stupid
    if(os_name[0] == '"' || os_name[0] == '\'')
        ++os_name;

    if((end = strchr(os_name, '"')) == 0)
        end = strchr(os_name, '\'');
    if(end)
        *end = 0;

    if(_os_arch) {
        const size_t len = strlen(os_name) + strlen(name.machine) + 4;
        res->string = malloc(len);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, len, "%s (%s)", os_name, name.machine);
    }
    else {
        res->string = strdup(os_name);
        if(res->string == NULL)
            res->retval = ERR_OOM;
    }
#endif // __ANDROID__
#endif // __APPLE__

    return res;
}
