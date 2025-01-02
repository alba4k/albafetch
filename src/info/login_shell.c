#include <stdlib.h>
#include <libgen.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current login shell
int login_shell(char *dest) {
    char *buf = getenv("SHELL");

    if(buf && buf[0]) {
        safe_strncpy(dest, _shell_path ? buf : basename(buf), 256);
        return 0;
    }

    return 1;
}
