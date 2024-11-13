#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <sys/utsname.h>
#include <stdio.h>

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
        snprintf(dest, 256, "%s (%s)", name.release, type);
    else
        strncpy(dest, name.release, 256);

    return 0;
}
