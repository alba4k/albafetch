#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>

#include "config.h"

/*
                   -`                 .     (whoami)@(/etc/hostname) 
                  .o+`                .     —————————————— 
                 `ooo/                .     Uptime: XXd XXh XXm
                `+oooo:               .     —————————————————— 
               `+oooooo:              .     OS: Arch Linux x86_64 
               -+oooooo+:             .     Kernel: (uname -r) 
             `/:-:++oooo+:            .     WM: i3 
            `/++++/+++++++:           .     Shell: fish
           `/++++++++++++++:          .     Terminal: kitty 
          `/+++ooooooooooooo/`        .     Packages: (pacman -Q | wc -l) 
         ./ooosssso++osssssso+`       .     ——————————————————————— 
        .oossssso-````/ossssss+`      .     Host: HP ProBook 440 G6 
       -osssssso.      :ssssssso.     .     CPU: i5-8265U (8) @ 1.5GHz [44.0°C] 
      :osssssss/        osssso+++.    .     GPU: WhiskeyLake-U GT2 [UHD Graphics 620] 
     /ossssssss/        +ssssooo/-    .     Memory: free --mebi | grep M | awk '{print $3 "MiB / " $2 "MiB"}
   `/ossssso+/:-        -:/+osssso+-  .
  `+sso+:-`                 `.-/+oso: .
 `++:.                           `-/+/.
 .`                                 `/.
*/

int main() {

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
        printf("%s%s", logo[0], spacing);
    }

    wait(NULL);

    close(pipes[1]);
    size_t len = read(pipes[0], username, 33);
    username[len - 1] = 0;

    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s@%s\n%s%s%s", username, hostname, logo[1], spacing, separator);

    // ******** uptime ********
    if(!fork()) {
        // some way to get the uptime
        exit(0);
    } else {
        printf("%s%sUptime:", logo[2], spacing);
    }
    wait(NULL);
    printf("\n%s%s%s", logo[3], spacing, separator);

    // ******** os ********
    static char OS_arch[10];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-m", NULL);
    }
    wait(NULL);

    close(pipes[1]);
    len = read(pipes[0], OS_arch, 10);
    OS_arch[len - 1] = 0;

    printf("%s%sOS: %s %s\n", logo[4], spacing, OS, OS_arch);

    // ******* kernel ********

    static char kernel[30];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("uname", "uname", "-r", NULL);
    } else {
        printf("%s%sKernel: ", logo[5], spacing);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], kernel, 30);
    kernel[len - 1] = 0;
    printf("%s\n", kernel);
    
    // ******** wm ********
    printf("%s%sWM: %s\n", logo[6], spacing, WM);

    // ******** shell ********

    // TODO: find a way lol
    printf("%s%sShell: %s\n", logo[7], spacing, shell);

    // ******** shell ********

    // TODO: find a way lol
    printf("%s%sTerminal: %s\n", logo[8], spacing, term);

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
        printf("%s%sPackages: ", logo[9], spacing);
    }
    wait(NULL);
    close(pipes[1]);

    len = read(pipes[0], packages, 10);
    packages[len - 1] = 0;

    printf("%s (pacman)\n%s%s%s", packages, logo[10], spacing, separator);

    // ******** host ********

    printf("%s%sHost: %s\n", logo[11], spacing, host);

    // ******** cpu ********

    printf("%s%sCPU: %s\n", logo[12], spacing, cpu);

    // ******** cpu ********

    printf("%s%sGPU: %s\n", logo[13], spacing, gpu);

    // ******** mem ********

    char used[8];
    char total[8];

    int pipes2[2];

    pipe(pipes);
    pipe(pipes2);
    if(!fork()) {           // this child gets the used memory
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        system("free --mebi | grep M | awk '{print $3}'");
        exit(0);
    } else if(!fork()) {    // this child gets the total memory
        close(pipes2[0]);
        dup2(pipes2[1], STDOUT_FILENO);

        system("free --mebi | grep M | awk '{print $2}'");
        exit(0);
    } else {
        printf("%s%sMemory: ", logo[14], spacing);
    }

    wait(NULL);
    wait(NULL);

    close(pipes[1]);
    len = read(pipes[0], used, 8);
    used[len - 1] = 0;

    close(pipes2[1]);
    len = read(pipes2[0], total, 8);
    total[len - 1] = 0;


    printf("%s / %s (%i%%)\n", used, total, (int)used / (int)total * 100);
}