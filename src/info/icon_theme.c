#include "info.h"

#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// get the current Icon Theme
int icon_theme(char *dest){ 
    int pipes[2];

    // try using gsettings
    // reading ~/.config/gtk-3.0/settings.ini could also be an option 
    if(access("/bin/gsettings", F_OK) == 0){
        if(pipe(pipes))
            return 1;

        if(fork() == 0) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("gsettings", "gsettings" , "get", "org.gnome.desktop.interface", "icon-theme", NULL); 
        }

        wait(0);
        close(pipes[1]);
        dest[read(pipes[0], dest, 256) - 1] = 0;
        close(pipes[0]);

        // cleanup
        if(dest[0] == '\'') {
            strcpy(dest, dest+1);

            char *ptr = strchr(dest, '\'');
            if(ptr)
                *ptr = 0;
        }

        return 0;
    }

    return 1;
}
