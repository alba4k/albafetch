#include <stdlib.h>
#include <libgen.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current login shell
int loginShell(char *dest) {
    char *buf = getenv("SHELL");

    if(buf != NULL) {
        safeStrncpy(dest, _shell_path ? buf : basename(buf), DEST_SIZE);
        return RET_OK;
    }

    return ERR_NO_INFO;
}
