#include <stdbool.h>
#include <stdlib.h>

#include "optdeps.h"

#ifdef GLIB_EXISTS
#include <glib-2.0/glib.h>
#else
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#endif // GLIB_EXISTS

// use g_find_program_in_path() if glib can be used
bool binaryInPath(const char *binary) {
#ifdef GLIB_EXISTS
    // valgrind complains about this, don't really know why
    gchar *program = g_find_program_in_path(binary);

    g_free(program);

    if(program == NULL)
        return false;

    return true;
#else
    char *path_env = getenv("PATH");
    if(path_env == NULL) {
        return false;
    }

    char *path = strdup(path_env);
    if(path == NULL) {
        return false;
    }

    char *dir = strtok(path, ":");
    while(dir) {
        size_t len = strlen(dir) + strlen(binary) + 2;
        char full_path[len];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, binary);

        if(access(full_path, X_OK) == 0) {
            free(path);
            return true;
        }

        dir = strtok(NULL, ":");
    }

    free(path);
    return false;
#endif // GLIB_EXISTS
}
