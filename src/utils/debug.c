#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "../config/config.h"
#include "../info/info.h"
#include "../utils/return.h"
#include "../utils/utils.h"
#include "debug.h"

void run_debug(unsigned *errors, double *time) {
    struct SModule {
        int (*func)(char *);
        char *name;
    };
    struct SModule arr[] = {{user, "user"},
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
                            {lightColors, "light_colors"}};

    *errors = 0;
    int return_value;
    char mem[DEST_SIZE];

    struct timeval start, end, start_all;
    gettimeofday(&start_all, NULL);

    for(unsigned long i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
        gettimeofday(&start, NULL);

        return_value = arr[i].func(mem);

        gettimeofday(&end, NULL);

        *time = ((end.tv_sec - start.tv_sec) * 1e6 + end.tv_usec - start.tv_usec) / 1e3;

        if(return_value == 0) {
            printf("\033[1m\033[32m%-12s\033[0m %-40s [\033[1m\033[36m\033[1m%.3f ms\033[0m]\n", arr[i].name, mem, *time);
        } else {
            printf("\033[1m\033[31m%-13s\033[0m 0x%-38x"
                   "[\033[1m\033[36m\033[1m%.3f ms\033[0m]\n",
                   arr[i].name, return_value, *time);
            ++(*errors);
        }
    }
    
    *time = ((end.tv_sec - start_all.tv_sec) * 1e6 + end.tv_usec - start_all.tv_usec) / 1e3;
}
