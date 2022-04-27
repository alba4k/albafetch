#include "config.h"
#include "info.h"

char *color = DEFAULT_COLOR;
char *bold = DEFAULT_BOLD;

int printLogo(const int line) {
    if(logo[line][0]) {
        printf("\n%s%s%s" SPACING, bold, logo[line], color);
        return line+1;
    } else {
        printf("\n%s%s%s" SPACING, bold, logo[2], color);
        return line;
    }
}

int main(const int argc, char **argv) {
    bool help = 0;
    int line = 3;

    // rtfm and stfu
    bool user_is_an_idiot = false;
    
    for(int i = 0; i < argc; i++) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help = 1;
        } else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color")) {
            if(argv[i+1]) {
                char * colors[9][2] = {
                    {"black", "\e[30m"},
                    {"red", "\e[31m"},
                    {"green", "\e[32m"},
                    {"yellow", "\e[33m"},
                    {"blue", "\e[34m"},
                    {"purple", "\e[35m"},
                    {"cyan", "\e[36m"},
                    {"gray", "\e[90m"},
                    {"white", "\e[97m"},
                };

                for (int j = 0; j < 9; ++j) {
                    if (!strcmp(argv[i+1], colors[j][0])) {
                        color = colors[j][1];
                        goto color;
                    }
                }

                fputs("\e[31m\e[1mERROR\e[0m\e[97m: invalid color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;

                color: ;
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[97m: --color requires a color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on")) {
                    bold = "\e[1m";
                } else if(!strcmp(argv[i+1], "off")) {
                    bold = "";
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m\e[97m: invalid value for --bold! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[97m: --bold requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo")) {
            if(argv[i+1]) {
                /*if(!strcmp(argv[i+1], "arch"))
                    logo = archlinux;
                else if(!strcmp(argv[i+1], "debian"))
                    logo = debian;
                else if(!strcmp(argv[i+1], "mint"))
                    logo = linuxmint;
                else 
                    logo = generic;*/
                for(int j = 0; j < sizeof(logos)/sizeof(logos[0]); j++)
                    if(!strcmp(logos[j][0], argv[i+1])) {
                        logo = logos[j];
                        goto logo_arg_found;
                    }
                    logo = logos[0];
                    logo_arg_found: ;
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[97m: --logo requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        }
    }

    if(user_is_an_idiot) return 1;

    if(!logo) {
        FILE *fp = fopen("/etc/os-release", "r");
        if(!fp) {
            return -1;
        }
        fseek(fp, 0, SEEK_END);
        size_t len = ftell(fp);
        rewind(fp);
        char *str = malloc(len + 1);
        str[fread(str, 1, len, fp)] = 0;
        const char *field = "ID=";
        char *os_id = strstr(str, field);
        if(!os_id) {
            fclose(fp);
            free(str);
            return -1;
        }
        os_id += strlen(field);
        char *end = strchr(os_id, '\n');
        if(!end) {
            fputs("\e[0m\e[97m[Unrecognized file content]", stderr);
            fclose(fp);
            free(str);
            return -1;
        }
        *end = 0;

        /*if(!strcmp(os_id, "arch"))
            logo = archlinux;
        else if(!strcmp(os_id, "debian"))
            logo = debian;
        else if(!strcmp(os_id, "linuxmint"))
            logo = linuxmint;
        else
            logo = generic;*/

        for(int i = 0; i < sizeof(logos)/sizeof(logos[0]); i++)
            if(!strcmp(logos[i][0], os_id)) {
                logo = logos[i];
                goto logo_found;
            }
            logo = logos[0];
            logo_found: ;

    }
    if(!color) {
        color = logo[1];
    }

    if(help) {  // print the help message if --help was used and exit
        printf("%s%salbafetch\e[0m\e[97m - a system fetch utility\n", color, bold);
        printf("\n%s%sFLAGS\e[0m\e[97m:\n", color, bold);
        printf("\t%s%s-h\e[0m\e[97m,%s%s --help\e[0m\e[97m:\t Print this help menu and exit\n", color, bold, color, bold);
        printf("\t%s%s-c\e[0m\e[97m,%s%s --color\e[0m\e[97m:\t Change the output color (default: \e[36mcyan\e[0m\e[97m]) [\e[30mblack\e[0m\e[97m, \e[31mred\e[0m\e[97m, \e[32mgreen\e[0m\e[97m, \e[33myellow\e[0m\e[97m, \e[34mblue\e[0m\e[97m, \e[35mpurple\e[0m\e[97m, \e[36mcyan\e[0m\e[97m, \e[90mgray\e[0m\e[97m, \e[97mwhite\e[0m\e[97m]\n", color, bold, color, bold);
        printf("\t%s%s-b\e[0m\e[97m,%s%s --bold\e[0m\e[97m:\t Specifies if bold should be used in colored parts (default: \e[1mon\e[0m\e[97m]) [\e[1mon\e[0m\e[97m, off]\n", color, bold, color, bold);
        printf("\t%s%s-l\e[0m\e[97m,%s%s --logo\e[0m\e[97m:\t Changes the logo that will be displayed (default: arch) [arch, debian, linuxmint, endeavouros, everything else defaults to linux]\n", color, bold, color, bold);

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\e[97m\n", color, bold);

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
    while(logo[line][0]) {
        line = printLogo(line);
    }
    printf("\e[0m\n");

    return 0;
}
