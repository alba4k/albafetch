#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#endif // __linux__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the parent process name (usually the shell)
SInfo *shell(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char shell_name[16];
#ifdef __linux__
    char path[32];

    sprintf(path, "/proc/%d/cmdline", getppid());

    FILE *fp = fopen(path, "r");
    if(fp) {
        char shell[64];
        size_t read = fread(shell, sizeof(*shell), sizeof(shell) - 1, fp);
        fclose(fp);

        if(read > 0)
            shell[read - 1] = 0;
        else {
            res->retval = ERR_NO_INFO;
            return res;
        }

        safeStrncpy(shell_name, basename(shell), sizeof(shell_name));
        res->string = strdup(_shell_path ? shell : shell_name);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
    }
    else {
        res->retval = ERR_NO_INFO;
        return res;
    }
#else // could probably be used on linux as fallback?
    char *shell = getenv("SHELL");
    if(shell == NULL) {
        res->retval = ERR_NO_INFO;
        return res;
    }
    if(shell[0] == 0) {
        res->retval = ERR_NO_INFO + 0x10;
        return res;
    }

    safeStrncpy(shell_name, basename(shell), sizeof(shell_name));
    res->string = strdup(_shell_path ? shell : shell_name);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
#endif
    if(_shell_version) {
        char version[32] = "";
        if(strcmp(shell_name, "fish") == 0) {
            char *argv[] = {"fish", "--version", NULL}; // using `fish -c "echo $FISH_VERSION"` would be slower
            execCmd(version, sizeof(version), argv);
            if(!version[0])
                return res;

            memmove(version, version + 14, strlen(version + 14) + 1);
        }
        else if(strcmp(shell_name, "make") == 0) {
            char *argv[] = {"make", "--version", NULL};
            execCmd(version, sizeof(version), argv);
            if(!version[0])
                return res;

            memmove(version, version + 9, strlen(version + 9) + 1);
            char *end = strchr(version, '\n');
            if(end != NULL)
                *end = 0;
        }
        else if(strcmp(shell_name, "bash") == 0) {
            char *argv[] = {"bash", "-c", "echo $BASH_VERSION", NULL};
            execCmd(version, sizeof(version), argv);
            if(!version[0])
                return res;

            char *end = strchr(version, '(');
            if(end != NULL)
                *end = 0;
        }

        if(version[0] == 0)
            return res;

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(version) + 2);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, " ");
            strcat(res->string, version);
        }
    }

    return res;
}
