#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <limits.h>         // used to get max hostname lenght
#include <stdint.h>
#include <string.h>

#include "config.h"

void separator() {      // prints a separator
    fputs(SEPARATOR, stdout);
}

void title(char *color) {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s\e[0m@%s%s\e[0m", getlogin(), color, hostname);
}

void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("Hostname:\e[0m %s", hostname);
}

void user() {           // get the current login
    printf("User:\e[0m %s", getlogin());
}

void uptime() {         // prints the uptime
    struct sysinfo info;
    sysinfo(&info);

    unsigned int secs = info.uptime;            // total uptime in seconds
    unsigned int days = secs/86400;
    unsigned int hours = secs/3600 - days*24;
    unsigned int mins = secs/60 - days*1440 - hours*60;
    unsigned int sec = secs - days*86400 - hours*3600 - mins*60;

    printf("%-11s\e[0m", "Uptime:");
    if(days) {
        printf("%ud ", days);     // print the number of days passed if more than 0
    }
    if(hours) {
        printf("%uh ", hours);       // print the number of days passed if more than 0
    }
    if(mins) {
        printf("%um ", mins);        // print the number of minutes passed if more than 0
    }
    else if(secs < 60) {
        printf("%us", sec);         // print the number of seconds passed if more than 0
    }
}

void os() {             // prints the os name + arch
    struct utsname name;
    uname(&name);

    static char os_name[128];
    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "{ lsb_release -ds; } | tr -d '\n' | tr -d '\"'", NULL);
    } else {
        printf("%-11s\e[0m", "OS:");
    }
    wait(NULL);

    close(pipes[1]);
    size_t len = read(pipes[0], os_name, 128);
    os_name[len - 1] = 0;

    close(pipes[0]);
    printf("%s %s", os_name, name.machine);
}

void kernel() {         // prints the kernel version
    struct utsname name;
    uname(&name);
    
    printf("%-11s\e[0m%s", "Kernel:" , name.release);
}

void desktop() {        // prints the current desktop environment
        printf("%-11s\e[0m%s", "Desktop:", getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP
}

void shell() {          // prints the user default shell
    printf("%-11s\e[0m%s", "Shell:", getenv("SHELL"));        // $SHELL
}

void term() {           // prints the current terminal
    printf("%-11s\e[0m%s", "Terminal:", getenv("TERM"));     // $TERM
}

void packages() {       // prints the number of installed packages
    // using pacman and only pacman, get away with it

    char packages[10];

    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "pacman -Q | wc -l", NULL);        // using "pacman --query" to list the installed packages; using "wc --lines" to get the number of lines (wordcount)
    } else {
        printf("%-11s\e[0m", "Packages:");
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 9) - 1] = 0;

    close(pipes[0]);
    printf("%s (pacman)", packages);
}

void host() {           // prints the current host machine
    printf("%-11s\e[0m%s", "Host:", HOST);
}

void cpu() {            // prints the current CPU
    // could get it from /proc/cpuinfo
    printf("%-11s\e[0m%s", "CPU:", CPU);
}

void gpu() {            // prints the current CPU
    printf("%-11s\e[0m%s", "GPU:", GPU);
}

void memory() {         // prints the used memory in the format used MiB / total MiB (XX%)
    struct sysinfo info;
    sysinfo(&info);

    unsigned long total = info.totalram;
    char used_str[15];

    // would be way more elegant, but leaves a slight gap? idk, I could also use /proc/meminfo
    //unsigned long used = total - info.freeram - info.bufferram - info.sharedram;

    //printf(COLOR "%s" SPACING "Memory:\e[0m %lu MiB / %lu MiB (%lu%%)\n", logo[14], used/1048576, total/1048576, (used * 100) / (1048576*total));

    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "free --byte | grep M | awk '{print $3}'", NULL);  // using free to count the used memory, 3rd arg
    } else {
        printf("%-11s\e[0m", "Memory:");
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], used_str, 14);
    used_str[read(pipes[0], used_str, 14) - 1] = 0;
    uint64_t used = atol(used_str);

    close(pipes[0]);
    
    printf("%ld MiB / %lu MiB (%ld%%)", used/1048576, total/1048576, (used * 100) / total);
}

void public_ip() {      // get the public IP adress - WORK IN PROGRESS

}

void local_ip() {      // get the local IP adress - WORK IN PROGRESS
    
}

int main(const int argc, char **argv) {
    int help = 0;
    for(int i = 0; i < argc; i++) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help = 1;
        } else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1],"black")) {
                    strcpy(color, "\e[30m");
                } else if(!strcmp(argv[i+1],"red")) {
                    strcpy(color, "\e[31m");
                } else if(!strcmp(argv[i+1],"green")) {
                    strcpy(color, "\e[32m");
                } else if(!strcmp(argv[i+1],"yellow")) {
                    strcpy(color, "\e[33m");
                } else if(!strcmp(argv[i+1],"blue")) {
                    strcpy(color, "\e[34m");
                } else if(!strcmp(argv[i+1],"pink")) {
                    strcpy(color, "\e[35m");
                } else if(!strcmp(argv[i+1],"cyan")) {
                    strcpy(color, "\e[36m");
                } else if(!strcmp(argv[i+1],"white")) {
                    strcpy(color, "\e[38m");
                } else {
                    puts("ERROR: invalid color! Use --help for more info");
                    return 0;
                }
            } else {
                puts("ERROR: --color requires a color! Use --help for more info");
                return 1;
            }
        } else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on")) {
                    strcpy(bold, "\e[1m");
                } else if(!strcmp(argv[i+1], "off")) {
                    strcpy(bold, "");
                } else {
                    puts("ERROR: invalid value for --bold! Use --help for more info");

                    return 0;
                }
            } else {
                puts("ERROR: --bold requires a value! Use --help for more info");

                return 0;
            }
        }
    }

    strcat(color, bold);

    if(help) {  // print the help message if --help was used and exit
        printf("%salbafetch\e[0m - a system fetch utility\n", color);
        printf("\n%sFLAGS:\e[0m\n", color);
        printf("\t%s-h\e[0m,%s --help\e[0m:\t Print this help menu and exit\n", color, color);
        printf("\t%s-c\e[0m,%s --color\e[0m:\t Change the output color (default: cyan) [black, red, green, yellow, blue, pink, cyan, white]\n", color, color);
        printf("\t%s-b\e[0m,%s --bold\e[0m:\t Specify if bold should be used in colored parts (default: on) [on, off]", color, color);
        printf("\nReport a bug: %shttps://github.com/alba4k/albafetch/issues\e[0m\n", color);

        return 0;
    }

    printf("%s%s" SPACING, color, logo[0]);
    title(color);

    printf("%s\n%s" SPACING, color, logo[1]);
    separator();

    printf("%s\n%s" SPACING, color, logo[2]);
    uptime();

    printf("%s\n%s" SPACING, color, logo[3]);
    separator();

    printf("%s\n%s" SPACING, color, logo[4]);
    os();

    printf("%s\n%s" SPACING, color, logo[5]);
    kernel();

    printf("%s\n%s" SPACING, color, logo[6]);
    desktop();

    printf("%s\n%s" SPACING, color, logo[7]);
    shell();

    printf("%s\n%s" SPACING, color, logo[8]);
    term();

    printf("%s\n%s" SPACING, color, logo[9]);
    packages();

    printf("%s\n%s" SPACING, color, logo[10]);
    separator();

    printf("%s\n%s" SPACING, color, logo[11]);
    host();

    printf("%s\n%s" SPACING, color, logo[12]);
    cpu();

    printf("%s\n%s" SPACING, color, logo[13]);
    gpu();

    printf("%s\n%s" SPACING, color, logo[14]);
    memory();

    // ******** remaining lines of the logo ********

    for(int i = 15; i < sizeof(logo) / sizeof(char*); i++) {
        printf("%s\n%s\e[0m", color, logo[i]);
    }
    printf("\n");
}