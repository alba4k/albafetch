#pragma once

#ifndef INFO_H
#define INFO_H

#include <limits.h>
#include <stdbool.h>

#ifndef HOST_NAME_MAX
    #ifdef _POSIX_HOST_NAME_MAX
        #define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
    #else
        #define HOST_NAME_MAX 128
    #endif
#endif

// Not sure if this 
#ifndef LOGIN_NAME_MAX
    #define LOGIN_NAME_MAX HOST_NAME_MAX
#endif

typedef struct {
    bool print_shell_path;
} Config;

extern Config config;

int user(char *dest);

int hostname(char *dest);

int uptime(char *dest);

int os(char *dest);

int kernel(char *dest);

int desktop(char *dest);

int shell(char *dest);

int login_shell(char *dest);

#endif // INFO_H
