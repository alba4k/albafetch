#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../utils/wrappers.h"
#include "info.h"

// get the current GTK Theme
SInfo *gtkTheme(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    // try using GTK_THEME (faster)
    char *theme = getenv("GTK_THEME");
    if(theme) {
        res->string = strdup(theme);
        if(res->string != NULL)
            return res;
    }

    // try using gsettings (fallback)
    // reading ~/.config/gtk-3.0/settings.ini could also be an option
    if(!binaryInPath("gsettings")) {
        res->retval = ERR_UNSUPPORTED;
        return res;
    }

    char buf[64] = "";
    char *args[] = {"gsettings", "get", "org.gnome.desktop.interface", "gtk-theme", NULL};
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
