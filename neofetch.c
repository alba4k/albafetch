#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

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
     /ossssssss/        +ssssooo/-    .     Memory: (free --mebi | grep M; 3nd element) / (free --mebi | grep M; 2nd element) (7%) 
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
        static char spacing[4] = "    ";                // defines the spacing between the logo and the infos
        static char separator[] = "—————————————————";  // defines what is used as separator between sections

    static char hostname[33];
    if(!fork()) {
        execlp("whoami", "whoami", NULL);
    } else if(!fork()) {
        static char hostname[33];
        FILE *fpointer = fopen("/etc/hostname", "r");
        fgets(hostname, 33, fpointer);
        fclose(fpointer);
        exit(0);
    } else {
        printf("%s%s", logo[0], spacing);
    }
    wait(NULL);
    wait(NULL);

    printf("alba4k@%s\n%s%s%s\n", hostname, logo[1], spacing, separator);
}