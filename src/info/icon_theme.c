#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "info.h"
#include "../optdeps/optdeps.h"
#include "../utils/utils.h"

// get the current Icon Theme
int icon_theme(char *dest){ 
    // try using gsettings
    // reading ~/.config/gtk-3.0/settings.ini could also be an option 
    if(binary_in_path("gsettings")) {
        char buf[256] = "";
        char *args[] = {"gsettings", "get", "org.gnome.desktop.interface", "icon-theme", NULL};
        exec_cmd(buf, 256, args);

        // cleanup
        if(buf[0] != 0) {
            if(buf[0] == '\'') {
                memmove(buf, buf+1, strlen(buf));

                char *ptr = strchr(buf, '\'');
                if(ptr)
                    *ptr = 0;
            }

            strncpy(dest, buf, 256);
            return 0;
        }
    }

    return 1;
}
