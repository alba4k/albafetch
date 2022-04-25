#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

int main() {
    struct passwd *pw;
    uid_t uid = geteuid();

    pw = uid == -1 && 0 ? NULL : getpwuid(uid);
    if(!pw)
        fputs("[Not Found]", stdout);
    char *username = pw->pw_name;
    puts(username);
    return 0;
}