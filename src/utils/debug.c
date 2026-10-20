#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "../info/info.h"
#include "../utils/return.h"
#include "../utils/utils.h"
#include "debug.h"

int runDebug(double *time) {
    struct SModule {
        SInfo *(*func)(void);
        char *name;
    };
    struct SModule arr[] = {
        {user, "user"},
        {hostname, "hostname"},
        {uptime, "uptime"},
        {os, "os"},
        {kernel, "kernel"},
        {desktop, "desktop"},
        {gtkTheme, "gtk_theme"},
        {iconTheme, "icon_theme"},
        {cursorTheme, "cursor_theme"},
        {shell, "shell"},
        {loginShell, "login_shell"},
        {term, "term"},
        {packages, "packages"},
        {host, "host"},
        {bios, "bios"},
        {cpu, "cpu"},
        {gpu, "gpu"},
        {memory, "memory"},
        {swap, "swap"},
        {publicIp, "public_ip"},
        {localIp, "local_ip"},
        {pwd, "pwd"},
        {date, "date"},
        {battery, "battery"},
        {colors, "colors"},
        {lightColors, "light_colors"},
    };

    int errors = 0;
    SInfo *info = NULL;

    struct timeval start, end, start_all;
    gettimeofday(&start_all, NULL);

    for(unsigned long i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
        gettimeofday(&start, NULL);

        info = arr[i].func();

        gettimeofday(&end, NULL);

        *time = (((end.tv_sec - start.tv_sec) * 1e6) + end.tv_usec - start.tv_usec) / 1e3;

        if(info == NULL) {
            printf("\033[1m\033[31m%-13s\033[0m NULL"
                   "[\033[1m\033[36m\033[1m%.3f ms\033[0m]\n",
                   arr[i].name, *time);
            errors++;
        }

        if(info->retval == RET_OK) {
            printf("\033[1m\033[32m%-12s\033[0m %-42s%s[\033[1m\033[36m\033[1m%.3f ms\033[0m]\n", arr[i].name, info->string,
                   (!strcmp(arr[i].name, "colors") || !strcmp(arr[i].name, "light_colors")) ? "                  " : "", *time);
        }
        else {
            printf("\033[1m\033[31m%-13s\033[0m 0x%-38x"
                   "[\033[1m\033[36m\033[1m%.3f ms\033[0m]\n",
                   arr[i].name, info->retval, *time);
            errors++;
        }
        fflush(stdout);

        free(info->string);
        free(info);
    }

    *time = (((end.tv_sec - start_all.tv_sec) * 1e6) + end.tv_usec - start_all.tv_usec) / 1e3;

    return errors;
}
