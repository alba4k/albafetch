#include "../config/config.h"
#include "../utils/wrappers.h"
#include "info.h"

#include <string.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// get the current desktop environment
SInfo *desktop(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifdef __APPLE__
    res->string = strdup("Aqua");
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
#else
    char *de_name = getenv("SWAYSOCK")                        ? "Sway" :
                    (de_name = getenv("XDG_CURRENT_DESKTOP")) ? de_name :
                    (de_name = getenv("DESKTOP_SESSION"))     ? de_name :
                    getenv("KDE_SESSION_VERSION")             ? "KDE" :
                    getenv("GNOME_DESKTOP_SESSION_ID")        ? "GNOME" :
                    getenv("MATE_DESKTOP_SESSION_ID")         ? "MATE" :
                    getenv("TDE_FULL_SESSION")                ? "Trinity" :
                                                                // strcmp("linux", getenv("TERM") == 0 ? "none" :      // running in tty
                                                                NULL;
    if(de_name == NULL) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    res->string = strdup(de_name);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }

    if(_de_version) {
        char version[32] = "";
        if(strcmp(de_name, "Hyprland") == 0) {
            char *argv[] = {"hyprctl", "version", NULL};
            execCmd(version, sizeof(version), argv);

            if(strlen(version) > 9) {
                memmove(version, version + 9, strlen(version + 9) + 1);
                char *end = strchr(version, ' ');
                if(end)
                    *end = 0;
            }
        }
        else if(strcmp(de_name, "KDE") == 0) {
            char *argv[] = {"plasmashell", "--version", NULL};
            execCmd(version, sizeof(version), argv);

            if(strlen(version) > 11)
                memmove(version, version + 11, strlen(version + 11) + 1);
        }
        else if(strcmp(de_name, "GNOME") == 0) {
            char *argv[] = {"gnome-shell", "--version", NULL};
            execCmd(version, sizeof(version), argv);

            if(strlen(version) > 11)
                memmove(version, version + 11, strlen(version + 11) + 1);
        }

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(version) + 2);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, " ");
            strcat(res->string, version);
        }
    }

    if(_de_type) {
        char buf[16] = "";

        if(getenv("WAYLAND_DISPLAY"))
            strcpy(buf, " (Wayland)");
        else if((de_name = getenv("XDG_SESSION_TYPE"))) {
            if(de_name[0] == 0)
                return res;
            de_name[0] = toupper(de_name[0]);

            snprintf(buf, sizeof(buf), " (%s) ", de_name);
        }

        if(buf[0] == 0)
            return res;

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(buf) + 1);
        if(tmpptr == NULL)
            return res;

        res->string = tmpptr;
        strcat(res->string, buf);
    }
#endif

    return res;
}
