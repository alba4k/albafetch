#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "info.h"
#include "../optdeps/optdeps.h"
#include "../utils/wrappers.h"

// get the current GTK Theme
int gtk_theme(char *dest){ 
    char *theme = getenv("GTK_THEME");

    // try using GTK_THEME (faster)
    if(theme) {
        safe_strncpy(dest, theme, 256);

        return 0;
    }

    // try using gsettings (fallback)
    // reading ~/.config/gtk-3.0/settings.ini could also be an option 
    if(binary_in_path("gsettings")) {
        char buf[256] = "";
        char *args[] = {"gsettings", "get", "org.gnome.desktop.interface", "gtk-theme", NULL};
        exec_cmd(buf, 256, args);

        // cleanup
        if(buf[0] != 0) {
            if(buf[0] == '\'') {
                memmove(buf, buf+1, strlen(buf));

                char *ptr = strchr(buf, '\'');
                if(ptr)
                    *ptr = 0;
            }

            safe_strncpy(dest, buf, 256);
            return 0;
        }
    }

    return 1;
}
