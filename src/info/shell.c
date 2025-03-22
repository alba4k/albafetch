#include <libgen.h>
#include <stdlib.h>

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#endif // __linux__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the parent process name (usually the shell)
int shell(char *dest) {
#ifdef __linux__
    char path[32];

    sprintf(path, "/proc/%d/cmdline", getppid());

    FILE *fp = fopen(path, "r");
    if(fp) {
        char shell[DEST_SIZE];
        shell[fread(shell, 1, DEST_SIZE - 1, fp)] = 0;
        fclose(fp);

        if(shell[0] == '-') { // cmdline is "-bash" when login shell
            safe_strncpy(dest, _shell_path ? shell + 1 : basename(shell + 1), DEST_SIZE);
            return RET_OK;
        }

        safe_strncpy(dest, _shell_path ? shell : basename(shell), DEST_SIZE);
        return RET_OK;
    }
#endif

    char *shell = getenv("SHELL");
    if(shell == NULL)
        return ERR_NO_INFO;
    if(shell[0] == 0)
        return ERR_NO_INFO + 0x10;

    safe_strncpy(dest, _shell_path ? shell : basename(shell), DEST_SIZE);

    return RET_OK;
}
