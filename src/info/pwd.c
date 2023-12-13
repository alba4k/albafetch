#include "info.h"
#include "../utils.h"

#include <string.h>

#include <unistd.h>

// get the current working directory
int pwd(char *dest) {
    if(!(pwd_path)) {
        char buf[256];

        if(!getcwd(buf, 256))
            return 1;

        strncpy(dest, buf, 256);
    }

    if(!getcwd(dest, 256))
        return 1;

    return 0;
}
