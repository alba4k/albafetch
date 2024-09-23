#include "info.h"

#define _GNU_SOURCE

#include <string.h>

#include <unistd.h>
#include <limits.h>

// idk why but this is sometimes not defined
#ifndef HOST_NAME_MAX
    #ifdef _POSIX_HOST_NAME_MAX
        #define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
    #else
        #define HOST_NAME_MAX 255
    #endif
#endif

// print the machine hostname
int hostname(char *dest) {
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    char *ptr = strstr(hostname, ".local");
    if(ptr)
        *ptr = 0;

    strncpy(dest, hostname, 256);

    return 0;
}
