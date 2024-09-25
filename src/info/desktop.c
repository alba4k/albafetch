#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// get the current desktop environment
int desktop(char *dest) {
    #ifdef __APPLE__
        strcpy(dest, "Aqua");
    #else
        char *desktop = getenv("SWAYSOCK") ? "Sway" :
                            (desktop = getenv("XDG_CURRENT_DESKTOP")) ? desktop :
                            (desktop = getenv("DESKTOP_SESSION")) ? desktop :
                            getenv("KDE_SESSION_VERSION") ? "KDE" :
                            getenv("GNOME_DESKTOP_SESSION_ID") ? "GNOME" :
                            getenv("MATE_DESKTOP_SESSION_ID") ? "MATE" :
                            getenv("TDE_FULL_SESSION") ? "Trinity" :
                            // strcmp("linux", getenv("TERM") == 0 ? "none" :      // running in tty
                            NULL;
        if(desktop == NULL)
            return 1;

        strcpy(dest, desktop);

        if(de_type) {
            if(getenv("WAYLAND_DISPLAY"))
                strncat(dest, " (Wayland)", 255-strlen(dest));
            else if((desktop = getenv("XDG_SESSION_TYPE"))) {
                if(desktop[0] == 0)
                    return 0;
                desktop[0] = toupper(desktop[0]);
                
                char buf[32];
                snprintf(buf, 32, " (%s) ", desktop);
                strncat(dest, buf, 255-strlen(dest));
            }
        }
    #endif

    return 0;
}
