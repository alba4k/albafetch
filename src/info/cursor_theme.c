#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "info.h"
#include "../optdeps/optdeps.h"
#include "../utils/wrappers.h"

// get the current Cursor Theme
int cursorTheme(char *dest) {
    // try using gsettings
    // reading ~/.config/gtk-3.0/settings.ini could also be an option
    if(binaryInPath("gsettings")) {
        char buf[DEST_SIZE] = "";
        char *args[] = {"gsettings", "get", "org.gnome.desktop.interface", "cursor-theme", NULL};
        execCmd(buf, DEST_SIZE, args);

        // cleanup
        if(buf[0] != 0) {
            if(buf[0] == '\'') {
                memmove(buf, buf + 1, strlen(buf));

                char *ptr = strchr(buf, '\'');
                if(ptr)
                    *ptr = 0;
            }

            safeStrncpy(dest, buf, DEST_SIZE);
            return RET_OK;
        }
    }

    return ERR_UNSUPPORTED;
}
