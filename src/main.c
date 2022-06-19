#include "info.h"
#include "logos.h"
#include "stdlib.h"

char *separator_string;
char *dash;
char *dash_color;

bool print_cpu_freq;

bool default_bold = true;
char default_color[33] = "";
char default_logo_color[33];
char default_logo[33] = "";

char **logo;

char spacing[32] = "    ";

// default config
Config config = {
    "\e[0m\e[37m------------------",    // separator
    ":",                                // dash
    "\e[37m",                           // dash_color
    true,                               // print_cpu_freq
    true,                               // print_gpu_arch
    true,                               // align stats
    "",                                 // color
    "\e[1m",                            // bold
    // Labels:
    "Hostname",                         // hostname
    "User",                             // user
    "Uptime",                           // uptime
    "OS",                               // os
    "Kernel",                           // kernel
    "Desktop",                          // desktop
    "Shell",                            // shell
    "Terminal",                         // terminal
    "Packages",                         // packages
    "Host",                             // host
    "BIOS",                             // bios
    "CPU",                              // cpu
    "GPU",                              // gpu
    "Memory",                           // memory
    "Publ. IP",                         // public IP
    "Loc. IP",                          // local IP
};

void parse_config() {
    // really bad code here, you don't need to look

    char path[LOGIN_NAME_MAX + 32];
    snprintf(path, LOGIN_NAME_MAX + 33, "%s/.config/albafetch.conf", getenv("HOME"));

    FILE *fp = fopen(path, "r");
    if(!fp) {
        return;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    // unescape
    char *ptr = conf;
    while(ptr=strchr(ptr, '\\')) {
        memmove(ptr, ptr+1, strlen(ptr+1));
        switch(*ptr) {
            default: case '\\':
                break;
            case 'n':
                *ptr = '\n';
                break;
            case 'e':
                *ptr = 033;
                break;
        }
    ++ptr;
    }

    // remove comments
    char *ptr2 = ptr = conf;
    while(ptr = strchr(ptr, ';')) {
        ptr2 = strchr(ptr, '\n');
        if(!ptr2) {
            *ptr = 0;   // end of file, no new lines, terminating string
            break;
        }
        memmove(ptr, ptr2+1, strlen(ptr2+1));
    }
    ptr = ptr2 = conf;
    while(ptr = strchr(ptr, '#')) {
        ptr2 = strchr(ptr, '\n');
        if(!ptr2) {
            *ptr = 0;   // end of file, no new lines, terminating string
            break;
        }
        memmove(ptr, ptr2+1, strlen(ptr2+1));
    }
    
    // spacing
    if(ptr = strstr(conf, "spacing")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(spacing, ptr);
            *ptr2 = '"';
        }
    }

    // separator
    if(ptr = strstr(conf, "separator")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.separator, ptr);
            *ptr2 = '"';
        }
    }

    // dash
    if(ptr = strstr(conf, "dash")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.dash, ptr);
            *ptr2 = '"';
        }
    }

    // dash_color
    if(ptr = strstr(conf, "dash_color")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.dash_color, ptr);
            *ptr2 = '"';
        }
    }

    // print_cpu_freq
    if(ptr = strstr(conf, "print_cpu_freq")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            config.print_cpu_freq = !strcmp(ptr, "true");
            *ptr2 = '"';
        }
    }

    // print_gpu_arch
    if(ptr = strstr(conf, "print_gpu_arch")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            config.print_gpu_arch = !strcmp(ptr, "true");
            *ptr2 = '"';
        }
    }

    // align_infos
    if(ptr = strstr(conf, "align_infos")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            config.align_infos = !strcmp(ptr, "true");
            *ptr2 = '"';
        }
    }
    
    // color
    if(ptr = strstr(conf, "default_color")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.color, ptr);
            strcpy(default_color, ptr);
            *ptr2 = '"';
        }
    }

    // bold
    if(ptr = strstr(conf, "default_bold")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.bold, strcmp(ptr, "true") ? "" : "\e[1m");
            default_bold = config.bold;
            *ptr2 = '"';
        }
    }

    // logo
    if(ptr = strstr(conf, "default_logo")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            if(*ptr) {
                for(int i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
                    if(!strcmp(logos[i][0], ptr)) {
                        logo = (char**)logos[i];
                        strcpy(default_logo, logo[0]);
                    }
            }
            *ptr2 = '"';
        }
    }

    // LABELS
    // hostname
    if(ptr = strstr(conf, "hostname_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.hostname_label, ptr);
            *ptr2 = '"';
        }
    }

    // user
    if(ptr = strstr(conf, "user_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.user_label, ptr);
            *ptr2 = '"';
        }
    }

    // uptime
    if(ptr = strstr(conf, "uptime_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.uptime_label, ptr);
            *ptr2 = '"';
        }
    }

    // os
    if(ptr = strstr(conf, "os_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.os_label, ptr);
            *ptr2 = '"';
        }
    }

    // kernel
    if(ptr = strstr(conf, "kernel_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.kernel_label, ptr);
            *ptr2 = '"';
        }
    }

    // desktop
    if(ptr = strstr(conf, "desktop_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.desktop_label, ptr);
            *ptr2 = '"';
        }
    }

    // shell
    if(ptr = strstr(conf, "shell_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.shell_label, ptr);
            *ptr2 = '"';
        }
    }

    // terminal
    if(ptr = strstr(conf, "term_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.term_label, ptr);
            *ptr2 = '"';
        }
    }

    // packages
    if(ptr = strstr(conf, "packages_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.packages_label, ptr);
            *ptr2 = '"';
        }
    }

    // host
    if(ptr = strstr(conf, "host_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.host_label, ptr);
            *ptr2 = '"';
        }
    }

    // bios
    if(ptr = strstr(conf, "bios_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.bios_label, ptr);
            *ptr2 = '"';
        }
    }

    // cpu
    if(ptr = strstr(conf, "cpu_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.cpu_label, ptr);
            *ptr2 = '"';
        }
    }

    // gpu
    if(ptr = strstr(conf, "gpu_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.gpu_label, ptr);
            *ptr2 = '"';
        }
    }
    
    // memory
    if(ptr = strstr(conf, "mem_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.mem_label, ptr);
            *ptr2 = '"';
        }
    }

    // public IP
    if(ptr = strstr(conf, "pub_ip_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.pub_ip_label, ptr);
            *ptr2 = '"';
        }
    }

    // local IP
    if(ptr = strstr(conf, "loc_ip_label")) {
        if(ptr = strchr(ptr, '"')) {
            ++ptr;
            ptr2 = strchr(ptr, '"');
            *ptr2 = 0;
            strcpy(config.loc_ip_label, ptr);
            *ptr2 = '"';
        }
    }

    free(conf);
}

int printLogo(const int line) {
    if(logo[line][0]) {
        printf("%s%s%s%s", config.bold, logo[line], spacing, config.color);
        return line+1;
    } else {
        printf("%s%s%s%s", config.bold, logo[2], spacing, config.color);
        return line;
    }
}

int main(const int argc, const char **argv) {
    parse_config();

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
                        strcpy(config.color, colors[j][1]);
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
                    strcpy(config.bold, "\e[1m");
                else if(!strcmp(argv[i+1], "off"))
                    strcpy(config.bold, "");
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
        } else if(!strcmp(argv[i], "--align") || !strcmp(argv[i], "-a")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on"))
                    config.align_infos = true;
                else if(!strcmp(argv[i+1], "off"))
                    config.align_infos = false;
                else {
                    fputs("\e[31m\e[1mERROR\e[0m\e[37m: invalid value for --align! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m\e[37m: --align requires a value! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        }
    }

    if(user_is_an_idiot)
        return 1;

    // set logo and color if not defined yet
    if(!logo) {
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
    if(!config.color[0])
        strcpy(config.color, logo[1]);

    if(help) {  // print the help message if --help was used and exit
        printf("%s%salbafetch\e[0m\e[37m - a system fetch utility\n",
               config.color, config.bold);

        printf("\n%s%sFLAGS\e[0m\e[37m:\n",
               config.color, config.bold);

        printf("\t%s%s-h\e[0m\e[37m,%s%s --help\e[0m\e[37m:\t Print this help menu and exit\n",
               config.color, config.bold, config.color, config.bold);

        printf("\t%s%s-c\e[0m\e[37m,%s%s --color\e[0m\e[37m:\t Change the output color (%s%s\e[0m\e[37m)\n"
               "\t\t\t   [\e[30mblack\e[0m\e[37m, \e[31mred\e[0m\e[37m, \e[32mgreen\e[0m\e[37m, \e[33myellow\e[0m\e[37m,"
               " \e[34mblue\e[0m\e[37m, \e[35mpurple\e[0m\e[37m, \e[36mcyan\e[0m\e[37m, \e[90mgray\e[0m\e[37m,"
               " \e[37mwhite\e[0m\e[37m]\n",
               config.color, config.bold, config.color, config.bold, default_color[0] ? default_color : logo[1], default_color[0] ? "default" : "logo default");

        printf("\t%s%s-b\e[0m\e[37m,%s%s --bold\e[0m\e[37m:\t Specifies if bold should be used in colored parts (default: %s\e[0m)\n"
               "\t\t\t   [\e[1mon\e[0m\e[37m, off]\n",
               config.color, config.bold, config.color, config.bold, default_bold ? "\e[1mon" : "off");
        
        printf("\t%s%s-l\e[0m\e[37m,%s%s --logo\e[0m\e[37m:\t Changes the logo that will be displayed (%s)\n"
               "\t\t\t   [linux, apple, arch, arch_small, debian, linuxmint, endeavouros, ubuntu]\n"
               "\t\t\t   [parrot, manjaro, fedora, neon, pop, gentoo, windows]\n",
               config.color, config.bold, config.color,config. bold, default_logo[0] ? default_logo : "OS default");

        printf("\t%s%s-a\e[0m\e[37m, %s%s--align\e[0m\e[37m:\t Alignes the infos if set (default: %s\e[0m)\n"
               "\t\t\t   [on, off]\n", config.color, config.bold, config.color, config.bold, config.align_infos ? "on" : "off");

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\n",
               config.color, config.bold);

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
