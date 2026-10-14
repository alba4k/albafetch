#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "info.h"
#include "../utils/wrappers.h"

// idk why but this is sometimes not defined
#ifndef HOST_NAME_MAX
#ifdef _POSIX_HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#else
#define HOST_NAME_MAX 255
#endif
#endif

// print the machine hostname
SInfo *hostname(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char hostname[HOST_NAME_MAX + 1] = "";
    if(gethostname(hostname, sizeof(hostname)) != 0 || hostname[0] == '\0') {
        res->retval = ERR_NO_INFO;
        return res;
    }

    char *ptr = strstr(hostname, ".local");
    if(ptr)
        *ptr = 0;

    res->string = strdup(hostname);
    if(res->string == NULL)
        res->retval = ERR_OOM;

    return res;
}
