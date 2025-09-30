#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#endif // __linux__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the parent process name (usually the shell)
int shell(char *dest) {
    char shell_name[16];
#ifdef __linux__
    char path[32];

    sprintf(path, "/proc/%d/cmdline", getppid());

    FILE *fp = fopen(path, "r");
    if(fp) {
        char shell[DEST_SIZE];
        shell[fread(shell, 1, DEST_SIZE - 1, fp)] = 0;
        fclose(fp);

        if(shell[0] == '-') { // cmdline is "-bash" when login shell
            safeStrncpy(shell_name, basename(shell+1), sizeof(shell_name));
            safeStrncpy(dest, _shell_path ? shell + 1 : shell_name, DEST_SIZE);
            return RET_OK;
        }

        safeStrncpy(shell_name, basename(shell), sizeof(shell_name));
        safeStrncpy(dest, _shell_path ? shell : shell_name, DEST_SIZE);
    }
    else
        return ERR_NO_INFO;
#else
    char *shell = getenv("SHELL");
    if(shell == NULL)
        return ERR_NO_INFO;
    if(shell[0] == 0)
        return ERR_NO_INFO + 0x10;
    
    safeStrncpy(shell_name, basename(shell), sizeof(shell_name));
    safeStrncpy(dest, _shell_path ? shell : shell_name, DEST_SIZE);
#endif
    if(_shell_version) {
        char version[32] = "";
        if(strcmp(shell_name, "fish") == 0) {
            char *argv[] = {"fish", "--version", NULL}; // using `fish -c "echo $FISH_VERSION"` would be slower
            execCmd(version, sizeof(version), argv);
            strncat(dest, " ", DEST_SIZE - strlen(dest));
            strncat(dest, version+14, DEST_SIZE - strlen(dest));
        }
        else if(strcmp(shell_name, "make") == 0) {
            char *argv[] = {"make", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            char *end = strchr(version + 9, '\n');
            if(end != NULL) {
                *end = 0;
                strncat(dest, " ", DEST_SIZE - strlen(dest));
                strncat(dest, version + 9, DEST_SIZE - strlen(dest));
            }
        }
        else if(strcmp(shell_name, "bash") == 0) {
            char *argv[] = {"bash", "-c", "echo $BASH_VERSION", NULL};
            execCmd(version, sizeof(version), argv);
            char *end = strchr(version, '(');
            if(end != NULL) {
                *end = 0;
                strncat(dest, " ", DEST_SIZE - strlen(dest));
                strncat(dest, version, DEST_SIZE - strlen(dest));
            }
        }
    }

    return RET_OK;
}
