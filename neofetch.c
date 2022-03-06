#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <limits.h>

#include "config.h"

int main() {

    struct sysinfo info;
    sysinfo(&info);

    // ******** first part ********
    static char hostname[HOST_NAME_MAX + 1];
    static char username[33];   // 32 characters max

    int pipes[2];

    pipe(pipes);    
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("whoami", "whoami", NULL);
    } else {
        printf(COLOR "%s\e[0m" SPACING, logo[0]);
    }

    wait(NULL);

    close(pipes[1]);
    size_t len = read(pipes[0], username, 33);
    username[len - 1] = 0;

    gethostname(hostname, HOST_NAME_MAX + 1);

    printf(COLOR "%s\e[0m@" COLOR "%s\n%s\e[0m" SPACING SEPARATOR, username, hostname, logo[1]);

    // ******** uptime ********

    unsigned int secs = info.uptime;
    unsigned int days = secs/86400;
    unsigned int hours = secs/3600 - days*24;
    unsigned int mins = secs/60 - days*864 - hours*60;

    printf(COLOR "%s" SPACING "Uptime:\e[0m ", logo[2]);
    if(days) {
        printf("%id, ", info.uptime/864000);    // print the number of days passed if more than 0
    }
    printf("%ih, %im\n", hours, mins);
    
    // some way to use the uptime

    // ******** os ********
    static char OS_arch[10];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-m", NULL);
    } else {
        printf(COLOR "%s\e[0m" SPACING SEPARATOR COLOR "%s" SPACING "OS:\e[0m ", logo[3], logo[4]);
    }
    wait(NULL);

    close(pipes[1]);
    len = read(pipes[0], OS_arch, 10);
    OS_arch[len - 1] = 0;

    printf(OS "%s\n", OS_arch);

    // ******* kernel ********

    static char kernel[30];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-r", NULL);
    } else {
        printf(COLOR "%s" SPACING "Kernel:\e[0m ", logo[5]);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], kernel, 30);
    kernel[len - 1] = 0;
    printf("%s\n", kernel);
    
    // ******** wm ********
    printf(COLOR"%s" SPACING "WM:\e[0m " WM "\n", logo[6]);

    // ******** shell ********

    // TODO: find a way lol
    printf(COLOR "%s" SPACING "Shell:\e[0m " SHELL "\n", logo[7]);

    // ******** term ********

    printf(COLOR "%s" SPACING "Terminal: \e[0m" TERM "\n", logo[8]);

    // ******** packages ********
    // using pacman, only pacman, get away with it

    char packages[10];

    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        system("pacman -Q | wc -l");
        exit(0);
    } else {
        printf(COLOR "%s" SPACING "Packages: ", logo[9]);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], packages, 10);
    packages[len - 1] = 0;

    printf(COLOR "\e[0m%s (pacman)\n" COLOR "%s\e[0m" SPACING SEPARATOR, packages, logo[10]);

    // ******** host ********

    printf(COLOR "%s" SPACING "Host:\e[0m " HOST "\n", logo[11]);

    // ******** CPU ********

    printf(COLOR "%s" SPACING "CPU:\e[0m " CPU "\n", logo[12]);

    // ******** GPU ********

    printf(COLOR "%s" SPACING "GPU:\e[0m " GPU "\n", logo[13]);

    // ******** mem ********

    printf(COLOR "%s" SPACING "Memory:\e[0m ", logo[14]);

    unsigned long total = (info.totalram)/1048576;
    unsigned long used = total - info.freeram/1048576;
    printf("%lu MiB / %lu MiB (%lu%%)\n", used, total, (used * 100) / total);

    // ******** remaining lines of the logo ********

    for(int i = 15; i < sizeof(logo) / sizeof(char*); i++) {
        printf(COLOR "%s\n\e[0m", logo[i]);
    }
}