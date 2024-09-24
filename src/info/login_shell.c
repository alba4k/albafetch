#include "info.h"
#include "../utils.h"


#include <string.h>
#include <stdlib.h>
#include <libgen.h>

// get the current login shell
int login_shell(char *dest) {
    char *buf = getenv("SHELL");

    if(buf && buf[0]) {
        strncpy(dest, shell_path ? buf : basename(buf), 256);
        return 0;
    }

    return 1;
}
