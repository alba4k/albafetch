#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "../config/config.h"
#include "info.h"

// print the running kernel version (uname -r)
SInfo *kernel(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    struct utsname name;
    uname(&name);
    char *ptr = name.release, *type = NULL;

    if(config.kernel_type)
        while((ptr = strchr(ptr, '-')))
            type = ++ptr;

    if(config.kernel_short && (ptr = strchr(name.release, '-')))
        *ptr = 0;

    if(config.kernel_type && type) {
        size_t s = strlen(name.release) + strlen(type) + 4;
        res->string = malloc(s);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, s, "%s (%s)", name.release, type);
    }
    else {
        res->string = strdup(name.release);
        if(res->string == NULL)
            res->retval = ERR_OOM;
    }

    return res;
}
