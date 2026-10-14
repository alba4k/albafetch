#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "info.h"
#include "../utils/wrappers.h"

// get the current Icon Theme
SInfo *iconTheme(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    // try using gsettings
    // reading ~/.config/gtk-3.0/settings.ini could also be an option
    if(!binaryInPath("gsettings")) {
        res->retval = ERR_UNSUPPORTED;
        return res;
    }

    char buf[64] = "";
    char *args[] = {"gsettings", "get", "org.gnome.desktop.interface", "icon-theme", NULL};
    execCmd(buf, 64, args);

    // cleanup
    if(buf[0] == 0) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    if(buf[0] == '\'') {
        memmove(buf, buf + 1, strlen(buf));

        char *ptr = strchr(buf, '\'');
        if(ptr)
            *ptr = 0;
    }

    res->string = strdup(buf);
    if(res->string == NULL)
        res->retval = ERR_OOM;

    return res;
}
