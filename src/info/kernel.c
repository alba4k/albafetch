#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// print the running kernel version (uname -r)
int kernel(char *dest) {
    struct utsname name;
    uname(&name);
    char *ptr = name.release, *type = NULL;

    if(_kernel_type) {
        while((ptr = strchr(ptr, '-')))
            type = ++ptr;
    }

    if(_kernel_short) {
        if((ptr = strchr(name.release, '-')))
            *ptr = 0;
    }

    if(_kernel_type && type)
        snprintf(dest, DEST_SIZE, "%s (%s)", name.release, type);
    else
        safeStrncpy(dest, name.release, DEST_SIZE);

    return RET_OK;
}
