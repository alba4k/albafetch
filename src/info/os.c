#include "info.h"
#include "../queue.h"
#include "../utils.h"

#include <string.h>

#include <stdio.h>
#include <sys/utsname.h>

#ifdef __ANDROID__
#include <unistd.h>
#include <sys/wait.h>
#endif // __ANDROID__

// print the operating system name and architecture (uname -m)
int os(char *dest) {
    struct utsname name;
    uname(&name);

    #ifdef __APPLE__
        snprintf(dest, 256, "macOS %s", os_arch ? name.machine : "");
    #else
    #ifdef __ANDROID__
        int pipes[2];
        char version[16];

        if(pipe(pipes))
            return 1;
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("getprop", "getprop", "ro.build.version.release", NULL); 
        }

        wait(0);
        close(pipes[1]);
        version[read(pipes[0], version, 16) - 1] = 0;
        close(pipes[0]);

        snprintf(dest, 256, "Android %s%s%s", version, version[0] ? " " : "", os_arch ? name.machine : "");
    #else
        FILE *fp = fopen("/etc/os-release", "r");
        if(!fp) {
            fp = fopen("/usr/lib/os-release", "r");
            if(!fp)
                return 1;
        }

        char buf[64];
        char *os_name = buf;
        char *end;

        read_after_sequence(fp, "PRETTY_NAME", buf, 64);
        fclose(fp);

        if(!buf[0])
            return 1;

        if(os_name[0] == '"' || os_name[0] == '\'')
            ++os_name;
        
        if(!(end = strchr(os_name, '\n')))
            return 1;
        *end = 0;

        if((end = strchr(os_name, '"')))
            *end = 0;
        else if((end = strchr(os_name, '\'')))
            *end = 0;

        snprintf(dest, 256, "%s %s", os_name, os_arch ? name.machine : "");
    #endif // __ANDROID__
    #endif // __APPLE__

    return 0;
}
