// This file is not part of albafetch.
// All it does is check whether every single module is working, and what might not be working.
// Useful for checking which info I'm able to get on multiple systems.

#include <stdio.h>
#include <string.h>

#include "info.h"
#include "utils.h"

// Most of those aren't even needed
struct Config config;

int main() {
    int (*arr[])(char *) = {
        user,
        hostname,
        uptime,
        os,
        kernel,
        desktop,
        shell,
        login_shell,
        term,
        packages,
        host,
        bios,
        cpu,
        gpu,
        memory,
        public_ip,
        local_ip,
        pwd,
        date,
        colors,
        light_colors
    };

    unsigned errors = 0;
    int return_value;
    char mem[256];

    // just setting everything to 1
    config.options = 0xffffffffffffffff;
    // these are just defaults
    config.col_block_len = 3;
    strcpy(config.date_format, "%02d/%02d/%d %02d:%02d:%02d");

    for(unsigned long i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        if(!(return_value = arr[i](mem)))
            printf("%s\n", mem);
        else {
            printf("%lu returned %d\n", i, return_value);
            ++errors;
        }
    }

    printf("\n\033[1mDebug run finished with a total of %u errors.\033[0m\n", errors);

    return 0;
}
