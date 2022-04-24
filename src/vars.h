#ifndef LOGOS_H
#define LOGOS_H

// all logos should end wih a \0
// add more, i couldn't care less
// just remember to add them to --logo and --help in src/main.c

static char logos[][2][32][64] = {
{{"arch"}, {
    "\e[36m",                                   // this line defines the default text color 
    "                                     ",    // this line is just spaces. This line is printed when there is more line than text
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
    0                                           // last line is 0 to know when to stop printing
}},
{{"debian"}, {  
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
    0
}},
{{"linuxmint"}, {
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
    0
}},
{{"generic"}, {  
    "\e[90m",
    "                     ",
    "\e[90m        #####        ",
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
    0
}},
}
extern char *color;
extern char *bold;

#endif