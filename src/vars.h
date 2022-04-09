#ifndef LOGOS_H
#define LOGOS_H

extern char *color;
extern char *bold;

// all logos should end wih a \0
// add more, i couldn't care less
// just remember to add them to --logo and --help in src/main.c

static char *archlinux[] = {
    "                                      ",             // first line is just spaces. This line is printed when there is more line than text
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
    " .`                                 `/",
    0                                                   // last line is 0 to know when to spo printing
};

static char *debian[] = {
    "                            ",             // first line is just spaces. This line is printed when there is more line than text
    "       _,met$$$$$gg.        ",
    "    ,g$$$$$$$$$$$$$$$P.     ",
    "  ,g$$P\"     \"\"\"Y$$.\".      ",
    " ,$$P'              `$$$.   ",
    "',$$P       ,ggs.     `$$b: ",
    "`d$$'     ,$P\"'   .    $$$  ",
    " $$P      d$'     ,    $$P  ",
    " $$:      $$.   -    ,d$$'  ",
    " $$;      Y$b._   _,d$P'    ",
    " Y$$.    `.`\"Y$$$$P\"'       ",
    " `$$b      \"-.__            ",
    "  `Y$$                      ",
    "   `Y$$.                    ",
    "     `$$b.                  ",
    "       `Y$$b.               ",
    "          `\"Y$b._           ", 
    "              `\"\"\"       ",
    0                                           // last line is 0 to know when to spo printing
};

#endif