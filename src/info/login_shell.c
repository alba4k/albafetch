#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include "info.h"
#include "../config/config.h"

// get the current login shell
SInfo *loginShell(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char *buf = getenv("SHELL");
    if(buf == NULL) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    if(_shell_path)
        buf = basename(buf);

    res->string = strdup(buf);
    if(res->string == NULL)
        res->retval = ERR_OOM;

    return res;
}
