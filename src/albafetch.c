#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <limits.h>         // used to get max hostname lenght
#include <stdint.h>

#include "config.h"

void separator() {      // prints a separator
    printf(SEPARATOR);
}

void title() {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf(COLOR "%s\e[0m@" COLOR "%s", getlogin(), hostname);
}

void uptime() {         // prints the uptime
    struct sysinfo info;
    sysinfo(&info);

    unsigned int secs = info.uptime;            // total uptime in seconds
    unsigned int days = secs/86400;
    unsigned int hours = secs/3600 - days*24;
    unsigned int mins = secs/60 - days*864 - hours*60; 
    unsigned int sec = secs - days*86400 - hours*3600 - mins*60;

    printf("Uptime:\e[0m ");
    if(days) {
        printf("%ud ", hours);     // print the number of days passed if more than 0
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
    static char OS_arch[10];
    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-m", NULL);   // using "uname --machine" to get the cpu instruction set (x86_64?)
    } else {
        printf("OS:\e[0m ");
    }
    wait(NULL);

    close(pipes[1]);
    size_t len = read(pipes[0], OS_arch, 10);
    OS_arch[len - 1] = 0;

    close(pipes[0]);
    printf(OS " %s", OS_arch);
}

void kernel() {         // prints the kernel version
    static char kernel[30];
    int pipes[2];

    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-r", NULL);   // using "uname --kernel-release" to get the kernel version
    } else {
        printf("Kernel:\e[0m ");
    }
    wait(NULL);
    close(pipes[1]);

    size_t len = read(pipes[0], kernel, 30);
    kernel[len - 1] = 0;

    close(pipes[0]);
    printf("%s", kernel);
}

void desktop() {        // prints the current desktop environment
        printf("Desktop:\e[0m %s", getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP
}

void shell() {          // prints the user default shell
    printf("Shell:\e[0m %s",getenv("SHELL"));        // $SHELL
}

void term() {           // prints the current terminal
    printf("Terminal: \e[0m %s", getenv("TERM"));     // $TERM
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
        printf("Packages:\e[0m ");
    }
    wait(NULL);
    close(pipes[1]);

    size_t len = read(pipes[0], packages, 10);
    packages[len - 1] = 0;

    close(pipes[0]);
    printf("%s (pacman)", packages);
}

void host() {           // prints the current host machine
    printf("Host:\e[0m " HOST);
}

void cpu() {            // prints the current CPU
    // could get it from /proc/cpuinfo
    printf("CPU:\e[0m " CPU "");
}

void gpu() {            // prints the current CPU
    printf("GPU:\e[0m " GPU);
}

void memory() {         // prints the sued memory in the format used MiB / total MiB (XX%)
    struct sysinfo info;
    sysinfo(&info);

    unsigned long total = (info.totalram);
    char used_str[14];

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
        printf("Memory:\e[0m ");
    }
    wait(NULL);
    close(pipes[1]);

    size_t len = read(pipes[0], used_str, 14);
    used_str[len - 1] = 0;
    uint64_t used = atol(used_str); 
    close(pipes[0]);
    
    printf("%ld MiB / %lu MiB (%ld%%)", used/1048576, total/1048576, (used * 100) / total);
}

int main() {
    printf(COLOR "%s" SPACING, logo[0]);
    title();

    printf(COLOR "\n%s" SPACING, logo[1]);
    separator();

    printf(COLOR "\n%s" SPACING, logo[2]);
    uptime();

    printf(COLOR "\n%s" SPACING, logo[3]);
    separator();

    printf(COLOR "\n%s" SPACING, logo[4]);
    os();

    printf(COLOR "\n%s" SPACING, logo[5]);
    kernel();

    printf(COLOR "\n%s" SPACING, logo[6]);
    desktop();

    printf(COLOR "\n%s" SPACING, logo[7]);
    shell();

    printf(COLOR "\n%s" SPACING, logo[8]);
    term();
    
    printf(COLOR "\n%s" SPACING, logo[9]);
    packages();

    printf(COLOR "\n%s" SPACING, logo[10]);
    separator();

    printf(COLOR "\n%s" SPACING, logo[11]);
    host();

    printf(COLOR "\n%s" SPACING, logo[12]);
    cpu();

    printf(COLOR "\n%s" SPACING, logo[13]);
    gpu();

    printf(COLOR "\n%s" SPACING, logo[14]);
    memory();

    // ******** remaining lines of the logo ********

    for(int i = 15; i < sizeof(logo) / sizeof(char*); i++) {
        printf(COLOR "\n%s\e[0m", logo[i]);
    }
    printf("\n");
}