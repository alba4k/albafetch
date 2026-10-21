#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config/config.h"
#include "info.h"

// get the current working directory
SInfo *pwd(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->retval = RET_OK;

    res->string = getcwd(NULL, 0);

    if(res->string == NULL) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    if(!_pwd_path) {
        char *dir = basename(res->string);
        memmove(res->string, dir, strlen(dir) + 1);
    }

    return res;
}
