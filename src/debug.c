// This file is not part of albafetch.
// All it does is check whether every single module is working, and what might not be working.
// Useful for checking which info I'm able to get on multiple systems.

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "info.h"
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

    // just setting everything to 1
    config.options = 0xffffffffffffffff;
    // these are just defaults
    config.col_block_len = 3;
    strcpy(config.date_format, "%02d/%02d/%d %02d:%02d:%02d");

    // The time thing doesn't work lol
    // clock_t start = clock();
    for(unsigned long i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        if(!(return_value = arr[i].func(mem)))
            printf("\e[1m\e[32m%s\e[0m: %s\n", arr[i].name, mem);
        else {
            printf("\e[1m\e[31m%s\e[0m: %d\n", arr[i].name, return_value);
            ++errors;
        }
    }
    // clock_t end = clock();

    // printf("\n\033[1mDebug run finished with a total of %u errors. [Took %f ms]\033[0m\n", errors, (double)(end-start)/CLOCKS_PER_SEC*1000);
    printf("\n\033[1mDebug run finished with a total of %u errors.\033[0m\n", errors);
    

    return 0;
}
