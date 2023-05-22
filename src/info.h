#pragma once

#ifndef INFO_H
#define INFO_H

#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

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

int user(char *dest);

int hostname(char *dest);

int uptime(char *dest);

int os(char *dest);

int kernel(char *dest);

int desktop(char *dest);

int shell(char *dest);

int login_shell(char *dest);

int term(char *dest);

int packages(char *dest);

int host(char *dest);

int bios(char *dest);

int cpu(char *dest);

int gpu(char *dest);

int memory(char *dest);

int public_ip(char *dest);

int local_ip(char *dest);

int pwd(char *dest);

int date(char *dest);

int colors(char *dest);

int light_colors(char *dest);

#endif // INFO_H
