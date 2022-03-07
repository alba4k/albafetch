#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <limits.h>         // used to get max hostname lenght

#include "config.h"

int main() {

    struct sysinfo info;        // used to get memory, uptime
    sysinfo(&info);

    // ******** title ********
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

    unsigned int secs = info.uptime;            // total uptime in seconds
    unsigned int days = secs/86400;
    unsigned int hours = secs/3600 - days*24;
    unsigned int mins = secs/60 - days*864 - hours*60;

    printf(COLOR "%s" SPACING "Uptime:\e[0m ", logo[2]);
    if(days) {
        printf("%id, ", info.uptime/864000);    // print the number of days passed if more than 0
    }
    printf("%ih, %im\n", hours, mins);

    // ******** os ********
    static char OS_arch[10];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-m", NULL);   // using "uname --machine" to get the cpu instruction set (x86_64?)
    } else {
        printf(COLOR "%s\e[0m" SPACING SEPARATOR COLOR "%s" SPACING "OS:\e[0m ", logo[3], logo[4]);
    }
    wait(NULL);

    close(pipes[1]);
    len = read(pipes[0], OS_arch, 10);
    OS_arch[len - 1] = 0;

    printf(OS " %s\n", OS_arch);

    // ******* kernel ********

    static char kernel[30];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-r", NULL);   // using "uname --kernel-release" to get the kernel version
    } else {
        printf(COLOR "%s" SPACING "Kernel:\e[0m ", logo[5]);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], kernel, 30);
    kernel[len - 1] = 0;
    printf("%s\n", kernel);
    
    // ******** wm ********
    printf(COLOR"%s" SPACING "Desktop:\e[0m %s\n", logo[6], getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP

    // ******** shell ********

    printf(COLOR "%s" SPACING "Shell:\e[0m %s\n", logo[7], getenv("SHELL"));        // $SHELL

        // ******** term ********
    printf(COLOR "%s" SPACING "Terminal: \e[0m %s\n", logo[8], getenv("TERM"));     // $TERM

    // ******** packages ********
    // using pacman and only pacman, get away with it

    char packages[10];

    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        system("pacman -Q | wc -l");        // using "pacman --query" to list the installed packages
        exit(0);                            // using "wc --lines" to get the number of lines (wordcount)
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
    // could get it from /proc/cpuingo
    printf(COLOR "%s" SPACING "CPU:\e[0m " CPU "\n", logo[12]);

    // ******** GPU ********

    printf(COLOR "%s" SPACING "GPU:\e[0m " GPU "\n", logo[13]);

    // ******** mem ********

    unsigned long total = (info.totalram);
    char used_str[14];

    // would be way more elegant, but leaves a slight gap? idk, I could also use /proc/meminfo
    //unsigned long used = total - info.freeram - info.bufferram - info.sharedram;

    //printf(COLOR "%s" SPACING "Memory:\e[0m %lu MiB / %lu MiB (%lu%%)\n", logo[14], used/1048576, total/1048576, (used * 100) / (1048576*total));
    
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        system("free --byte | grep M | awk '{print $3}'");  // using free to count the used memory, 3rd arg
        exit(0);
    } else {
        printf(COLOR "%s" SPACING "Memory:\e[0m ", logo[14]);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], used_str, 14);
    used_str[len - 1] = 0;
    const unsigned int used = atoi(used_str); 

    printf("%i MiB / %lu MiB (%lu%%)\n", used/1048576, total/1048576, (used * 100) / (total*1048576));

    // ******** remaining lines of the logo ********

    for(int i = 15; i < sizeof(logo) / sizeof(char*); i++) {
        printf(COLOR "%s\n\e[0m", logo[i]);
    }
}