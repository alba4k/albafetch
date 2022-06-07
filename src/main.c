#include "config.h"
#include "info.h"
#include "logos.h"
#include "stdlib.h"

char *separator_string;
char *dash;
char *dash_color;

bool print_cpu_freq;

char *color;
char *bold;
char *default_logo;
char **logo;

char *host_name_label;
char *user_label;
char *uptime_label;
char *os_label;
char *kernel_label;
char *desktop_label;
char *shell_label;
char *term_label;
char *packages_label;
char *host_label;
char *bios_label;
char *cpu_label;
char *gpu_label;
char *mem_label;
char *pub_ip_label;
char *priv_ip_label;

char spacing[32] = "    ";

Config config = {
    "\e[0m\e[37m------------------",    // separator
    ":",                                // dash
    "\e[37m",                           // dash_color
};

int parse_config() {
    // really bad code here, you don't need to look

    char path[LOGIN_NAME_MAX + 32];
    snprintf(path, LOGIN_NAME_MAX + 32, "albafetch.conf");
    //snprintf(path, LOGIN_NAME_MAX + 33, "%s/.config/albafetch.conf", getenv("HOME"));

    FILE *fp = fopen(path, "r");
    if(!fp) {
        fputs("\e[31m\e[1mERROR\e[0m! Couldn't open the config!\n", stderr);
        return -1;
    }

    char *end;
    char line[64];
    char *ptr;

    while(fscanf(fp, "%[^\n] ", line) != EOF) {
        if(line[0] == ';' || line[0] == '#')
            continue;

        ptr = strstr(line, "spacing=\"");
        if(ptr) {
            ptr += 9;
            end = strchr(ptr, '"');
            if(!end) {
                fflush(stdout);
                fputs("\e[31m\e[1mERROR\e[0m! Couldn't parse the config!\n", stderr);
                fflush(stderr);
                return -1;
            }
            *end = 0;
            strcpy(spacing, ptr);
            continue;
        }

        ptr = strstr(line, "separator=\"");
        if(ptr) {
            ptr += 11;
            end = strchr(ptr, '"');
            if(!end) {
                fflush(stdout);
                fputs("\e[31m\e[1mERROR\e[0m! Couldn't parse the config!\n", stderr);
                fflush(stderr);
                return -1;
            }
            *end = 0;
            strcpy(config.separator, ptr);
            continue;
        }
    }

    color = DEFAULT_COLOR;
    bold = DEFAULT_BOLD;

    return 0;
}

int printLogo(const int line) {
    if(logo[line][0]) {
        printf("%s%s%s%s", bold, logo[line], spacing, color);
        return line+1;
    } else {
        printf("%s%s%s%s", bold, logo[2], spacing, color);
        return line;
    }
}

int main(const int argc, const char **argv) {
    if(parse_config())
        return 1;

    bool help = false;
    int line = 3;

    // rtfm and stfu
    bool user_is_an_idiot = false;
    
    // command line arguments
    for(int i = 0; i < argc; ++i) {
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
                    {"white", "\e[37m"},
                };

                for(int j = 0; j < 9; ++j) {
                    if(!strcmp(argv[i+1], colors[j][0])) {
                        color = colors[j][1];
                        goto color;
                    }
                }

                fputs("\e[31m\e[1mERROR\e[0m\e[37m: invalid color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;

                color: ;
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[37m: --color requires a color! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on"))
                    bold = "\e[1m";
                else if(!strcmp(argv[i+1], "off"))
                    bold = "";
                else {
                    fputs("\e[31m\e[1mERROR\e[0m\e[37m: invalid value for --bold! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[37m: --bold requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo")) {
            if(argv[i+1]) {
                for(int j = 0; j < sizeof(logos)/sizeof(logos[0]); ++j)
                    if(!strcmp(logos[j][0], argv[i+1])) {
                        logo = (char**)logos[j];
                        goto logo_arg_found;
                    }
                logo = (char**)logos[0];
                logo_arg_found: ;
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[37m: --logo requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        }
    }

    if(user_is_an_idiot)
        return 1;

    // set logo and color if not defined yet
    if(!logo) {
        if(DEFAULT_LOGO[0])
            for(int i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
                if(!strcmp(logos[i][0], DEFAULT_LOGO)) {
                    logo = (char**)logos[i];
                    goto logo_found;
                }
        #ifdef __APPLE__
            logo = (char**)logos[1];
            goto logo_found;
        #else
            FILE *fp = fopen("/etc/os-release", "r");
            if(!fp) {
                logo = (char**)logos[0];
                goto logo_found;
            }
            
            char os_id[32];
            read_after_sequence(fp, "ID", os_id, 32);
            fclose(fp);

            char *end = strchr(os_id, '\n');
            if(!end)
                return -1;
            *end = 0;

            for(int i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
                if(!strcmp(logos[i][0], os_id)) {
                    logo = (char**)logos[i];
                    goto logo_found;
                }
            logo = (char**)logos[0];

            logo_found: ;
        #endif
    }
    if(!color[0])
        color = logo[1];

    if(help) {  // print the help message if --help was used and exit
        printf("%s%salbafetch\e[0m\e[37m - a system fetch utility\n",
               color, bold);
        printf("\n%s%sFLAGS\e[0m\e[37m:\n",
               color, bold);
        printf("\t%s%s-h\e[0m\e[37m,%s%s --help\e[0m\e[37m:\t Print this help menu and exit\n",
               color, bold, color, bold);

        printf("\t%s%s-c\e[0m\e[37m,%s%s --color\e[0m\e[37m:\t Change the output color (%s%s\e[0m\e[37m)\n"
               "\t\t\t [\e[30mblack\e[0m\e[37m, \e[31mred\e[0m\e[37m, \e[32mgreen\e[0m\e[37m, \e[33myellow\e[0m\e[37m,"
               " \e[34mblue\e[0m\e[37m, \e[35mpurple\e[0m\e[37m, \e[36mcyan\e[0m\e[37m, \e[90mgray\e[0m\e[37m,"
               " \e[37mwhite\e[0m\e[37m]\n",
               color, bold, color, bold, DEFAULT_COLOR[0] ? DEFAULT_COLOR : logo[1], DEFAULT_COLOR[0] ? "default" : "logo default");

        printf("\t%s%s-b\e[0m\e[37m,%s%s --bold\e[0m\e[37m:\t Specifies if bold should be used in colored parts (default: %s\e[0m)\n"
               "\t\t\t [\e[1mon\e[0m\e[37m, off]\n",
               color, bold, color, bold, DEFAULT_BOLD[0] ? "\e[1mon" : "off");
        
        printf("\t%s%s-l\e[0m\e[37m,%s%s --logo\e[0m\e[37m:\t Changes the logo that will be displayed (%s)\n"
               "\t\t\t [linux, apple, arch, arch_small, debian, linuxmint, endeavouros, ubuntu]\n"
               "\t\t\t [parrot, manjaro, fedora, neon, pop, gentoo, windows]\n",
               color, bold, color, bold, DEFAULT_LOGO[0] ? DEFAULT_LOGO : "OS default");

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\n",
               color, bold);

        return 0;
    }

    void (*infos[])() = {
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

    for(size_t i = 0; i < info_lines; ++i) {
        line = printLogo(line);
        infos[i]();
        printf("\n");
    }

    // ******** remaining lines of the logo ********
    while(logo[line][0]) {
        line = printLogo(line);
        printf("\n");
    }
    printf("\e[0m");

    return 0;
}
