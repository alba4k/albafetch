#include <unistd.h>
#include <pwd.h>

#include "info.h"
#include "../utils/wrappers.h"

// print the current user
int user(char *dest) {
    struct passwd *pw;

    unsigned uid = geteuid();
    if((int)uid == -1) {
        // couldn't get UID
        return ERR_NO_INFO;
    }

    pw = getpwuid(uid);

    safe_strncpy(dest, pw->pw_name, DEST_SIZE);

    return RET_OK;
}
