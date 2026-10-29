#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "info.h"

// get the current cursor theme
SInfo *cursorTheme(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    
    char *theme = getenv("XCURSOR_THEME");
    if(theme) {
        res->string = strdup(theme);
        if(res->string) {
            res->retval = RET_OK;

            return res;
        }
    }

    res->string = getGtkVar("gtk-cursor-theme-name");
    res->retval = res->string == NULL ? ERR_GENERIC : RET_OK;

    return res;
}
