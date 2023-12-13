#include "info.h"

#include <string.h>

#include <unistd.h>
#include <limits.h>
#include <pwd.h>


// print the current user
int user(char *dest) {
    struct passwd *pw;

    unsigned uid = geteuid();
    if((int)uid == -1) {
        // couldn't get UID
        return 1;
    }

    pw = getpwuid(uid);

    strncpy(dest, pw->pw_name, 255);

    return 0;
}
