#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "info.h"

// get the current icon cheme
SInfo *iconTheme(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = getGtkVar("gtk-icon-theme-name");
    res->retval = res->string == NULL ? ERR_GENERIC : RET_OK;

    return res;
}
