#include "info.h"
#include "../config/config.h"

#define _GNU_SOURCE

#include <string.h>

#include <stdlib.h>

#ifdef __APPLE__
#include <libgen.h>
#endif // __APPLE__

// get the current login shell
int login_shell(char *dest) {
    char *buf = getenv("SHELL");

    if(buf && buf[0]) {
        strncpy(dest, shell_path ? buf : basename(buf), 256);
        return 0;
    }

    return 1;
}
