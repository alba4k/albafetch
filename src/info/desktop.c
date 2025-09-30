#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

#include <string.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// get the current desktop environment
int desktop(char *dest) {
#ifdef __APPLE__
    strcpy(dest, "Aqua");
#else
    char *de_name = getenv("SWAYSOCK")            ? "Sway" :
        (de_name = getenv("XDG_CURRENT_DESKTOP")) ? de_name :
        (de_name = getenv("DESKTOP_SESSION"))     ? de_name :
        getenv("KDE_SESSION_VERSION")             ? "KDE" :
        getenv("GNOME_DESKTOP_SESSION_ID")        ? "GNOME" :
        getenv("MATE_DESKTOP_SESSION_ID")         ? "MATE" :
        getenv("TDE_FULL_SESSION")                ? "Trinity" :
                                                    // strcmp("linux", getenv("TERM") == 0 ? "none" :      // running in tty
                                                    NULL;
    if(de_name == NULL)
        return ERR_NO_INFO;

    strcpy(dest, de_name);
    
    if(_de_version) {
        char version[32] = "";
        if(strcmp(de_name, "Hyprland") == 0) {
            char *argv[] = {"hyprctl", "version", NULL};
            execCmd(version, sizeof(version), argv);
            char *end = strchr(version + 9, ' ');
            if(end != NULL) {
                *end = 0;
                strncat(dest, " ", DEST_SIZE - strlen(dest));
                strncat(dest, version + 9, DEST_SIZE - strlen(dest));
            }
        }
        else if(strcmp(de_name, "KDE") == 0) {
            char *argv[] = {"plasmashell", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            strncat(dest, version + 11, DEST_SIZE - strlen(dest));
        }
        else if(strcmp(de_name, "GNOME") == 0) {
            char *argv[] = {"gnome-shell", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            strncat(dest, version + 11, DEST_SIZE - strlen(dest));
        }
    }
    
    if(_de_type) {
        if(getenv("WAYLAND_DISPLAY"))
            strncat(dest, " (Wayland)", DEST_SIZE - strlen(dest));
        else if((de_name = getenv("XDG_SESSION_TYPE"))) {
            if(de_name[0] == 0)
                return RET_OK;
            de_name[0] = toupper(de_name[0]);

            char buf[32];
            snprintf(buf, 32, " (%s) ", de_name);
            strncat(dest, buf, DEST_SIZE - 1 - strlen(dest));
        }
    }
#endif

    return RET_OK;
}
