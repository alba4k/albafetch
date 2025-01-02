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
        return 1;
    }

    pw = getpwuid(uid);

    safe_strncpy(dest, pw->pw_name, 256);

    return 0;
}
