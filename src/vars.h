#ifndef LOGOS_H
#define LOGOS_H

// all logos should end wih a \0
// add more, i couldn't care less
// just remember to add them to --logo and --help in src/main.c

static char *logos[][32] = {
    {                                   // every logo needs to follow the following scheme:
        "linux",                                                // this first line contains the distro ID, taken from /etc/os-relase
        "\e[90m",                                               // default color for the printed text
        "                     ",                                // numbers of spaces to use when ran out of logo space
        "\e[90m        #####        ",                          // just the logo
        "\e[90m       #######       ",
        "\e[90m       ##\e[97mO\e[90m#\e[97mO\e[90m##       ",
        "\e[90m       #\e[33m#####\e[90m#       ",
        "\e[90m     ##\e[97m##\e[33m###\e[97m##\e[90m##     ",
        "\e[90m    #\e[97m##########\e[90m##    ",
        "\e[90m   #\e[97m############\e[90m##   ",
        "\e[90m   #\e[97m############\e[90m###  ",
        "\e[33m  ##\e[90m#\e[97m###########\e[90m##\e[33m#  ",
        "\e[33m######\e[90m#\e[97m#######\e[90m#\e[33m######",
        "\e[33m#######\e[90m#\e[97m#####\e[90m#\e[33m#######",
        "\e[33m  #####\e[90m#######\e[33m#####  ",
        ""                                                      // an empty string concludes the logo, to know when it should stop printing
    },
    {
        "arch",
        "\e[36m",
        "                                     ",
        "\e[36m                   -`                 ",
        "\e[36m                  .o+`                ",
        "\e[36m                 `ooo/                ",
        "\e[36m                `+oooo:               ",
        "\e[36m               `+oooooo:              ",
        "\e[36m               -+oooooo+:             ",
        "\e[36m             `/:-:++oooo+:            ",
        "\e[36m            `/++++/+++++++:           ",
        "\e[36m           `/++++++++++++++:          ",
        "\e[36m          `/+++ooooooooooooo/`        ",
        "\e[36m         ./ooosssso++osssssso+`       ",
        "\e[36m        .oossssso-````/ossssss+`      ",
        "\e[36m       -osssssso.      :ssssssso.     ",
        "\e[36m      :osssssss/        osssso+++.    ",
        "\e[36m     /ossssssss/        +ssssooo/-    ",
        "\e[36m   `/ossssso+/:-        -:/+osssso+-  ",
        "\e[36m  `+sso+:-`                 `.-/+oso: ",
        "\e[36m `++:.                           `-/+/",
        "\e[36m .`                                 `/",
        ""
    },
    {
        "debian",
        "\e[31m",
        "                            ",
        "\e[31m       _,met$$$$$gg.        ",
        "\e[31m    ,g$$$$$$$$$$$$$$$P.     ",
        "\e[31m  ,g$$P\"     \"\"\"Y$$.\".      ",
        "\e[31m ,$$P'              `$$$.   ",
        "\e[31m',$$P       ,ggs.     `$$b: ",
        "\e[31m`d$$'     ,$P\"'   .    $$$  ",
        "\e[31m $$P      d$'     ,    $$P  ",
        "\e[31m $$:      $$.   -    ,d$$'  ",
        "\e[31m $$;      Y$b._   _,d$P'    ",
        "\e[31m Y$$.    `.`\"Y$$$$P\"'       ",
        "\e[31m `$$b      \"-.__            ",
        "\e[31m  `Y$$                      ",
        "\e[31m   `Y$$.                    ",
        "\e[31m     `$$b.                  ",
        "\e[31m       `Y$$b.               ",
        "\e[31m          `\"Y$b._           ", 
        "\e[31m              `\"\"\"       ",
        ""
    },
    {
        "linuxmint",
        "\e[32m",
        "                                        ",
        "\e[97m             ...-:::::-...              ",
        "\e[97m          .-MMMMMMMMMMMMMMM-.           ",
        "\e[97m      .-MMMM\e[32m`..-:::::::-..`\e[97mMMMM-.       ",
        "\e[97m    .:MMMM\e[32m.:MMMMMMMMMMMMMMM:.\e[97mMMMM:.     ",
        "\e[97m   -MMM\e[32m-M---MMMMMMMMMMMMMMMMMMM.\e[97mMMM-    ",
        "\e[97m `:MMM\e[32m:MM`  :MMMM:....::-...-MMMM:\e[97mMMM:` ",
        "\e[97m :MMM\e[32m:MMM`  :MM:`  ``    ``  `:MMM:\e[97mMMM: ",
        "\e[97m.MMM\e[32m.MMMM`  :MM.  -MM.  .MM-  `MMMM.\e[97mMMM.",
        "\e[97m:MMM\e[32m:MMMM`  :MM.  -MM-  .MM:  `MMMM-\e[97mMMM:",
        "\e[97m:MMM\e[32m:MMMM`  :MM.  -MM-  .MM:  `MMMM:\e[97mMMM:",
        "\e[97m:MMM\e[32m:MMMM`  :MM.  -MM-  .MM:  `MMMM-\e[97mMMM:",
        "\e[97m.MMM\e[32m.MMMM`  :MM:--:MM:--:MM:  `MMMM.\e[97mMMM.",
        "\e[97m :MMM\e[32m:MMM-  `-MMMMMMMMMMMM-`  -MMM-\e[97mMMM: ",
        "\e[97m  :MMM\e[32m:MMM:`                `:MMM:\e[97mMMM:  ",
        "\e[97m   .MMM\e[32m.MMMM:--------------:MMMM.\e[97mMMM.   ",
        "\e[97m     '-MMMM\e[32m.-MMMMMMMMMMMMMMM-.\e[97mMMMM-'    ",
        "\e[97m       '.-MMMM\e[32m``--:::::--``\e[97mMMMM-.'      ",
        "\e[97m            '-MMMMMMMMMMMMM-'           ",
        "\e[97m               ``-:::::-``              ",
        ""
    },
    {
        "endeavouros",
        "\e[35m",
        "                                        ",
        "\e[31m                     ./\e[35mo\e[34m.               ",
        "\e[31m                   ./\e[35msssso\e[34m-             ",
        "\e[31m                 `:\e[35mosssssss+\e[34m-           ",
        "\e[31m               `:+\e[35msssssssssso\e[34m/.         ",
        "\e[31m             `-/o\e[35mssssssssssssso\e[34m/.       ",
        "\e[31m           `-/+\e[35msssssssssssssssso\e[34m+:`     ",
        "\e[31m         `-:/+\e[35msssssssssssssssssso«\e[34m+/.   ",
        "\e[31m       `.://\e[35mosssssssssssssssssssso\e[34m++-   ",
        "\e[31m      .://+\e[35mssssssssssssssssssssssso\e[34m++:  ",
        "\e[31m    .:///\e[35mossssssssssssssssssssssssso\e[34m++: ",
        "\e[31m  `:////\e[35mssssssssssssssssssssssssssso\e[34m+++.",
        "\e[31m`-////+\e[35mssssssssssssssssssssssssssso\e[34m++++-",
        "\e[31m `..-+\e[35moosssssssssssssssssssssssso\e[34m+++++/`",
        "\e[34m   ./++++++++++++++++++++++++++++++/:.  ",
        "\e[34m  `:::::::::::::::::::::::::------``    ",
        ""
    },
    {
        "ubuntu",
        "\e[31m",
        "                                        ",
        "\e[31m            .-/+oossssoo+/-.            ",
        "\e[31m        `:+ssssssssssssssssss+:`        ",
        "\e[31m      -+ssssssssssssssssssyyssss+-      ",
        "\e[31m    .ossssssssssssssssss\e[97mdMMMNy\e[31msssso.    ",
        "\e[31m   /sssssssssss\e[97mhdmmNNmmyNMMMMh\e[31mssssss/   ",
        "\e[31m  +sssssssss\e[97mhm\e[31myd\e[97mMMMMMMMNddddy\e[31mssssssss+  ",
        "\e[31m /ssssssss\e[97mhNMMM\e[31myh\e[97mhyyyyhmNMMMNh\e[31mssssssss/ ",
        "\e[31m.ssssssss\e[97mdMMMNh\e[31mssssssssss\e[97mhNMMMd\e[31mssssssss.",
        "\e[31m+ssss\e[97mhhhyNMMNy\e[31mssssssssssss\e[97myNMMMy\e[31msssssss+",
        "\e[31moss\e[97myNMMMNyMMh\e[31mssssssssssssss\e[97mhmmmh\e[31mssssssso",
        "\e[31moss\e[97myNMMMNyMMh\e[31msssssssssssssshmmmhssssssso",
        "\e[31m+ssss\e[97mhhhyNMMNy\e[31mssssssssssss\e[97myNMMMy\e[31msssssss+",
        "\e[31m.ssssssss\e[97mdMMMNh\e[31mssssssssss\e[97mhNMMMd\e[31mssssssss.",
        "\e[31m /ssssssss\e[97mhNMMM\e[31myh\e[97mhyyyyhdNMMMNh\e[31mssssssss/ ",
        "\e[31m  +sssssssss\e[97mdm\e[31myd\e[97mMMMMMMMMddddy\e[31mssssssss+  ",
        "\e[31m   /sssssssssss\e[97mhdmNNNNmyNMMMMh\e[31mssssss/   ",
        "\e[31m    .ossssssssssssssssss\e[97mdMMMNy\e[31msssso.    ",
        "\e[31m      -+sssssssssssssssss\e[97myyy\e[31mssss+-      ",
        "\e[31m        `:+ssssssssssssssssss+:`        ",
        "\e[31m            .-/+oossssoo+/-.            ",
        ""
    },
};

extern char *color;
extern char *bold;

#endif