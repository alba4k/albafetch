#include "config.h"
#include "info.h"

int printLogo(const int line) {
    if(logo[line]) {
        printf("%s\n%s%s" SPACING, color, bold, logo[line]);
        return line+1;
    } else {
        printf("\n%s%s%s" SPACING, logo[0], color, bold);
        return line;
    }
}

int main(const int argc, char **argv) {
    bool help = 0;
    int line = 1;

    // rtfm and stfu
    bool user_is_an_idiot = false;

    for(int i = 0; i < argc; i++) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help = 1;
        } else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color")) {
            if(argv[i+1]) {
                char * colors[8][2] = {
                    {"black", "\e[30m"},
                    {"red", "\e[31m"},
                    {"green", "\e[32m"},
                    {"yellow", "\e[33m"},
                    {"blue", "\e[34m"},
                    {"purple", "\e[35m"},
                    {"cyan", "\e[36m"},
                    {"white", "\e[39m"},
                };

                for (int j = 0; j < 8; ++j) {
                    if (!strcmp(argv[i+1], colors[j][0])) {
                        color = colors[j][1];
                        goto color;
                    }
                }

                fputs("\e[31m\e[1mERROR\e[0m: invalid color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;

                color: ;
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --color requires a color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on")) {
                    bold = "\e[1m";
                } else if(!strcmp(argv[i+1], "off")) {
                    bold = "";
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid value for --bold! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --bold requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "arch")) {
                    logo = archlinux;
                } else if(!strcmp(argv[i+1], "debian")) {
                    logo = debian;
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid value for --logo! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --logo requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        }
    }

    if(user_is_an_idiot) return 1;

    if(help) {  // print the help message if --help was used and exit
        printf("%s%salbafetch\e[0m - a system fetch utility\n", color, bold);
        printf("\n%s%sFLAGS\e[0m:\n", color, bold);
        printf("\t%s%s-h\e[0m,%s%s --help\e[0m:\t Print this help menu and exit\n", color, bold, color, bold);
        printf("\t%s%s-c\e[0m,%s%s --color\e[0m:\t Change the output color (default: cyan) [black, red, green, yellow, blue, purple, cyan, white]\n", color, bold, color, bold);
        printf("\t%s%s-b\e[0m,%s%s --bold\e[0m:\t Specifies if bold should be used in colored parts (default: on) [on, off]\n", color, bold, color, bold);
        printf("\t%s%s-l\e[0m,%s%s --logo\e[0m:\t Changes the logo that will be displayed (default: arch) [arch, debian]\n", color, bold, color, bold);

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\n", color, bold);

        return 0;
    }

    void (* infos[])() = {
        title,
        separator,
        uptime,
        separator, 
        os,
        kernel,
        desktop, 
        shell,
        term,
        packages, 
        separator,
        host,
        cpu,
        gpu,
        memory,
    };

    // The following line works because infos is declared on the stack,
    // so sizeof returns it's real size and not the size of a pointer.
    size_t info_lines = sizeof(infos) / sizeof(infos[0]);

    for (size_t i = 0; i < info_lines; i++) {
        line = printLogo(line);
        infos[i]();
    }

    // ******** remaining lines of the logo ********
    while(logo[line]) {
        line = printLogo(line);
    }
    printf("\e[0m\n");

    return 0;
}
