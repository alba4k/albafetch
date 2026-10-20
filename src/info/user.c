#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "info.h"
#include "../utils/wrappers.h"

// get the current user name
SInfo *user(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    unsigned uid = geteuid();
    if((int)uid == -1) {
        // couldn't get UID
        res->retval = ERR_NO_INFO;
        return res;
    }

    struct passwd *pw = getpwuid(uid);
    if(pw == NULL) {
        res->retval = ERR_NO_INFO + 0x10;
        return res;
    }

    res->string = strdup(pw->pw_name);
    if(res->string == NULL)
        res->retval = ERR_OOM;

    return res;
}
