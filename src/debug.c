// This file is not part of albafetch.
// All it does is check whether every single module is working, and what might not be working.
// Useful for checking which info I'm able to get on multiple systems.

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "info/info.h"
#include "utils.h"

#define name(f) call(#f, f) 
 
void call(const char *name, void (*f)(void)) { 
  printf("Calling %s()\n", name); 
  f(); 
} 

// Most of those aren't even needed
struct Config config;

int main() {

    struct Module {
        int (*func)(char *);
        char *name;
    };
    struct Module arr[] = {
        {user, "user"},
        {hostname, "hostname"},
        {uptime, "uptime"},
        {os, "os"},
        {kernel, "kernel"},
        {desktop, "desktop"},
        {gtk_theme, "gtk_theme"},
        {shell, "shell"},
        {login_shell, "login_shell"},
        {term, "term"},
        {packages, "packages"},
        {host, "host"},
        {bios, "bios"},
        {cpu, "cpu"},
        {gpu, "gpu"},
        {memory, "memory"},
        {public_ip, "public_ip"},
        {local_ip, "local_ip"},
        {pwd, "pwd"},
        {date, "date"},
        {colors, "colors"},
        {light_colors, "light_colors"}
    };

    unsigned errors = 0;
    int return_value;
    char mem[256];

    struct timeval start, end, start_all;
    double time;

    // just setting every option to 1
    config.options = 0xffffffffffffffff;
    // these are just defaults
    config.col_block_len = 3;
    strcpy(config.date_format, "%02d/%02d/%d %02d:%02d:%02d");

    gettimeofday(&start_all, NULL);

    for(unsigned long i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        gettimeofday(&start, NULL);

        return_value = arr[i].func(mem);

        gettimeofday(&end, NULL);

        time = ((end.tv_sec  - start.tv_sec) * 1e6 +
                 end.tv_usec - start.tv_usec) / 1e3;

        if(!return_value)
            printf("\033[1m\033[32m%-12s\033[0m %-40s [\033[1m\033[36m\033[1m%.3f ms\033[0m]\n", arr[i].name, mem, time);
        else {
            printf("\033[1m\033[31m%-12s\033[0m %d                                    "    
                   "[\033[1m\033[36m\033[1m%.3f ms\033[0m]\n", arr[i].name, return_value, time);
            ++errors;
        }
    }

    gettimeofday(&end, NULL);

    time = ((end.tv_sec  - start_all.tv_sec) * 1e6 +
                 end.tv_usec - start_all.tv_usec) / 1e3;

    printf("\n\033[1mDebug run finished with a total of %u errors.\033[0m [\033[1m\033[36m\033[1m%.3f ms\033[0m]\n", errors, time);
    
    return 0;
}
