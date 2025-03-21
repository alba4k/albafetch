#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the current working directory
int pwd(char *dest) {
    #ifdef __APPLE__
    char *pwd = getcwd(NULL, DEST_SIZE);
    #else
    char *pwd = getcwd(NULL, 0);
    #endif // __APPLE__

    if(pwd == NULL)
        return ERR_NO_INFO;
    
    safe_strncpy(dest, _pwd_path ? pwd : basename(pwd), DEST_SIZE);
    free(pwd);

    return RET_OK;
}
