#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "info.h"

// get the current GTK theme
SInfo *gtkTheme(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    char *end;
    
    char *theme = getenv("GTK_THEME");
    if(theme) {
        res->string = strdup(theme);
        if(res->string) {
            res->retval = res->string == RET_OK;
            
            goto cleanup;
        }
    }

    res->string = getGtkVar("gtk-theme-name");
    res->retval = res->string == NULL ? ERR_GENERIC : RET_OK;
    
    cleanup:
    if(res->string && (end = strchr(res->string, '+')))
        *end = 0;

    return res;
}
