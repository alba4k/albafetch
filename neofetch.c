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
        static char spacing[4] = "    ";    // defines the spacing between the logo and the infos

/*
    if(!fork()) {
        execlp("whoami", "whoami", NULL);
    } else if(!fork()) {
            execlp("cat", "cat", "/etc/hostname", NULL);
    }

    wait(NULL);
    wait(NULL);
*/
    static char hostname[33];
    if(!fork()) {
        execlp("whoami", "whoami", NULL);
    } else if(!fork()) {
        FILE *fpointer = fopen("/etc/hostname", "r");
        fgets(hostname, 33, fpointer);
        fclose(fpointer);
        exit(0);
    } else {
        printf("%s", logo[1]);
    }
    wait(NULL);
    wait(NULL);

    printf("%salba4k@%s\n", spacing, hostname);
    /*
    REM
    change the logic from

    [prende tutte le info in dei child paralleli]
    [stampa le linee dell'icona e le info]

    to

    [raccoglie le info e stampa la prima riga del logo contemporaneamente]
    [stampa le info]
    [ripete con la 2a riga]
    ...
    */
}