#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>

// TODO: get the uptime

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
    static char *logo[] = {
        "                   -`                 ",
        "                  .o+`                ",
        "                 `ooo/                ",
        "                `+oooo:               ",
        "               `+oooooo:              ",
        "               -+oooooo+:             ",
        "             `/:-:++oooo+:            ",
        "            `/++++/+++++++:           ",
        "           `/++++++++++++++:          ",
        "          `/+++ooooooooooooo/`        ",
        "         ./ooosssso++osssssso+`       ",
        "        .oossssso-````/ossssss+`      ",
        "       -osssssso.      :ssssssso.     ",
        "      :osssssss/        osssso+++.    ",
        "     /ossssssss/        +ssssooo/-    ",
        "   `/ossssso+/:-        -:/+osssso+-  ",
        "  `+sso+:-`                 `.-/+oso: ",
        " `++:.                           `-/+/",
        " .`                                 `/"
    };
    
    // CONFIGURATION OPTIONS:
        const static char spacing[5] = "    ";                      // defines the spacing between the logo and the infos
        const static char separator[19] = "------------------\n";   // defines what is used as separator between sections
        const static char OS[18] = "Arch Linux x86_64";             // idk how to implement it so get away with this
// ******** first part ********
    static char hostname[HOST_NAME_MAX + 1];   // 64 characters max
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
    username[--len] = 0;

    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s@%s\n%s%s%s", username, hostname, logo[1], spacing, separator);

// ******** uptime ********
    if(!fork()) {
        // some way to get the uptime
        exit(0);
    } else {
        printf("%s%s", logo[2], spacing);
    }
    wait(NULL);
    printf("Uptime: \n%s%s%s", logo[3], spacing, separator);

// ******** os, kernel ********
    printf("%s%sOS: %s\n", logo[4], spacing, OS);
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
    kernel[--len] = 0;
    printf("%s", kernel);
}