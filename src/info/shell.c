#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <libgen.h>
#include <stdlib.h>

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#endif // __linux__

// get the parent process name (usually the shell)
int shell(char *dest) {
    #ifdef __linux__
        char path[32];

        sprintf(path, "/proc/%d/cmdline", getppid());

        FILE *fp = fopen(path, "r");
        if(fp) {
            char shell[256];
            shell[fread(shell, 1, 255, fp)] = 0;
            fclose(fp);

            if(shell[0] == '-') { // cmdline is "-bash" when login shell
                strncpy(dest, _shell_path ? shell+1 : basename(shell+1), 256);
                return 0;
            }

            strncpy(dest, _shell_path ? shell : basename(shell), 256);
            return 0;
        }
    #endif

    char *shell = getenv("SHELL");
    if(shell == NULL)
        return 1;
    if(shell[0] == 0)
        return 1;
    
    strncpy(dest, _shell_path ? shell : basename(shell), 256);

    return 0;
}
