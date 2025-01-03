#include <stdlib.h>
#include <libgen.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current login shell
int login_shell(char *dest) {
    char *buf = getenv("SHELL");

    if(buf != NULL) {
        safe_strncpy(dest, _shell_path ? buf : basename(buf), DEST_SIZE);
        return 0;
    }

    return 1;
}
