#include "info.h"
#include "logos.h"
#include "stdlib.h"

/* TODO:
 * use strlen() to determine how far to --align ("%-%ds", max(strlen(a), strlen(b)) + 2 <-- NOT actual code)
 * !!! option to choose what order the infos are printed in ( modules {"a", "b"} in albafetch.conf)
 * --ascii for custom ascii art (conflicts with --logo) - work in progress (lines [64; 76]U[318; 356])
 * remove the lspci dependency for gpu()
 */

// CONFIGURATION OPTIONS:
char *separator_string; // what is used as separator between sections
char *dash; // default separator

// default config
Config config = {
    "\e[0m------------------",          // separator
    "",                                 // separator2
    ":",                                // dash
    true,                               // print_cpu_freq
    true,                               // print_cpu_brand
    true,                               // print_gpu_arch
    true,                               // print_mem_perc
    false,                               // align stats
    "",                                 // color
    "\e[1m",                            // bold
    // Labels:
    "",                                 // title_prefix
    "",                                 // col_prefix
    3,                                  // col_block_len
    "Hostname",                         // hostname
    "User",                             // user
    "Uptime",                           // uptime
    "OS",                               // os
    "Kernel",                           // kernel
    "Desktop",                          // desktop
    "Shell",                            // shell
    "Shell",                            // default_shell_label
    "Terminal",                         // terminal
    "Packages",                         // packages
    "Host",                             // host
    "BIOS",                             // bios
    "CPU",                              // cpu
    "GPU",                              // gpu
    "Memory",                           // memory
    "Publ. IP",                         // public IP
    "Loc. IP",                          // local IP
    "Directory"                         // pwd
};

bool print_cpu_freq;

bool default_bold = false;
char default_color[33] = "";
char default_logo[33] = "";

char **logo;

char spacing[32] = "    ";
char spacing_first[32] = "";
char spacing_last[32] = "";

/* Check what I wrote about logo_from_string - should be around line 320
 * also, this will be useful for determining how far to --align (to do comment at line 6)
int max(const int *nums, unsigned const int lenght) {
    if(!lenght) return 0;
    int current_max = *nums;

    for(int i = 1; i < lenght; ++i) {
        if(nums[i] > current_max)
            current_max = nums[i];
    }

    return current_max;
}
*/

void unescape(char *str) {
    // check every \ in the given string and unescape \n and \e
    while((str=strchr(str, '\\'))) {
        memmove(str, str+1, strlen(str+1)+1);
        switch(*str) {
            default: case '\\':
                break;
            case 'n':
                *str = '\n';
                break;
            case 'e':
                *str = '\e';
                break;
        }
    ++str;
    }
}

// in the following, a return code of 1 means success
bool parse_config_option(char* source, char *dest, char *field) {
    char *ptr;

    // looks for the keyword, checks the string between the following " "

    if((ptr = strstr(source, field))) {
        if((ptr = strchr(ptr, '"'))) {
            ++ptr;

            if(((field = strchr(ptr, '"'))) && ptr) {
                *field = 0;
                strcpy(dest, ptr);
                *field = '"';
                return 1;
            }
        }
    }

    return 0;
}

// in the following, a return code of 1 means success
bool parse_config_bool(char *source, bool *dest, char *field) {
    char *ptr;

    // looks for the keyword, checks the string between the following " "

    if((ptr = strstr(source, field))) {
        if((ptr = strchr(ptr, '"'))) {
            ++ptr; 

            if(((field = strchr(ptr, '"'))) && ptr) {
                *field = 0;
                *dest = !strcmp(ptr, "true");
                *field = '"';
                return 1;
            }
        }
    }

    return 0;
}

// parsing the file provided in path for config options
void parse_config(const char *path, const bool using_custom_config) {
    // really bad code here, you don't need to look

    FILE *fp = fopen(path, "r");
    if(!fp) {
        if(using_custom_config)
            fprintf(stderr, "\e[91m\e[1mWARNING\e[0m: failed to open the file specified using --config (%s), using defaults.\n", path);
        return;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    unescape(conf);

    // remove comments
    char *ptr = conf;
    char *ptr2 = conf;
    while((ptr = strchr(ptr, ';'))) {
        ptr2 = strchr(ptr, '\n');
        memmove(ptr, ptr2+1, strlen(ptr2+1)+1);
    }
    ptr = ptr2 = conf;
    while((ptr = strchr(ptr, '#'))) {
        ptr2 = strchr(ptr, '\n');
        memmove(ptr, ptr2+1, strlen(ptr2+1)+1);
    }
    
    // spacing
    parse_config_option(conf, spacing, "spacing");

    // spacing_first
    parse_config_option(conf, spacing_first, "spacing_first");

    // spacing_last
    parse_config_option(conf, spacing_last, "spacing_last");


    // separators
    parse_config_option(conf, config.separator, "separator");
    parse_config_option(conf, config.separator2, "separator2");

    // dash
    parse_config_option(conf, config.dash, "dash");

    // print_cpu_freq
    parse_config_bool(conf, &config.print_cpu_freq, "print_cpu_freq");

    // print_cpu_brand
    parse_config_bool(conf, &config.print_cpu_brand, "print_cpu_brand");

    // print_gpu_arch
    parse_config_bool(conf, &config.print_gpu_arch, "print_gpu_arch");

    // print_mem_perc
    parse_config_bool(conf, &config.print_mem_perc, "print_mem_perc");

    // align_infos
    parse_config_bool(conf, &config.align_infos, "align_infos");
    
    // color
    if(parse_config_option(conf, config.color, "default_color"))
        strcpy(default_color, config.color);

    // bold
    bool bold;
    if((default_bold = parse_config_bool(conf, &bold, "default_bold"))) {
        if(bold)
            strcpy(config.bold, "\e[1m");
        else
            strcpy(config.bold, "");
    }

    // logo
    if((ptr = strstr(conf, "default_logo"))) {
        if((ptr = strchr(ptr, '"'))) {
            ++ptr;
            if(((ptr2 = strchr(ptr, '"'))) && ptr) {
                *ptr2 = 0;
                for(int i = 0; i < sizeof(logos)/sizeof(*logos); ++i)
                    if(!strcmp(logos[i][0], ptr)) {
                        logo = (char**)logos[i];
                        strcpy(default_logo, *logo);
                    }
                *ptr2 = '"';
            }
        }
    }

    // col_block_len
    if((ptr = strstr(conf, "col_block_len"))) {
        if((ptr = strchr(ptr, '"'))) {
            ++ptr;
            if((ptr2 = strchr(ptr, '"'))) {
                *ptr2 = 0;
                config.col_block_len = atoi(ptr);
                *ptr2 = '"';
            }
        }
    }

    // LABELS
    // title
    parse_config_option(conf, config.title_prefix, "title_prefix");

    // colors
    parse_config_option(conf, config.col_prefix, "col_prefix");

    // hostname
    parse_config_option(conf, config.hostname_label, "hostname_label");

    // user
    parse_config_option(conf, config.user_label, "user_label");

    // uptime
    parse_config_option(conf, config.uptime_label, "uptime_label");

    // os
    parse_config_option(conf, config.os_label, "os_label");

    // kernel
    parse_config_option(conf, config.kernel_label, "kernel_label");

    // desktop
    parse_config_option(conf, config.desktop_label, "desktop_label");

    // shell (current)
    parse_config_option(conf, config.shell_label, "shell_label");

    // shell(default)
    parse_config_option(conf, config.default_shell_label, "def_shell_label");

    // terminal
    parse_config_option(conf, config.term_label, "term_label");

    // packages
    parse_config_option(conf, config.packages_label, "packages_label");

    // host
    parse_config_option(conf, config.host_label, "host_label");

    // bios
    parse_config_option(conf, config.bios_label, "bios_label");

    // cpu
    parse_config_option(conf, config.cpu_label, "cpu_label");

    // gpu
    parse_config_option(conf, config.gpu_label, "gpu_label");
    
    // memory
    parse_config_option(conf, config.mem_label, "mem_label");

    // public IP
    parse_config_option(conf, config.pub_ip_label, "pub_ip_label");

    // local IP
    parse_config_option(conf, config.loc_ip_label, "loc_ip_label");

    // pwd
    parse_config_option(conf, config.pwd_label, "pwd_label");

    free(conf);
}

int printLogo(const int line) {
    if(*logo[line]) {
        printf("%s%s%s", config.bold, logo[line], config.color);
        return line+1;
    } else {
        printf("%s%s%s", config.bold, logo[2], config.color);
        return line;
    }
}

/*  // I'm working on this part - I swear - I'm trying to implement getting a custom logo from a string (line per line)
void logo_from_string(char *str, char **dest) {
    unescape(str);

    char *ptr;
    int string_lengths[28];
    memset(string_lengths, 0, sizeof(string_lengths)/sizeof(string_lengths[0]));
    int line_count = 3;
    char placeholder_line[64] = "";
    char *temp_logo[32] = {
        "custom",
        "",
    };

    // place the lines in the array
    // TODO fix this loop, it's broken AF, it just works as a segfault farm, lol
    while((ptr = strchr(str, '\n'))) {
        if(line_count > 31) // prevent buffer overflow
            break;

        *ptr = 0;
        temp_logo[line_count++] = str;
        string_lengths[line_count-4] = (int)strlen(str);
        *ptr = '\n';

        str = ptr+1;
    }

    for(int i = 0; i < max(string_lengths, sizeof(string_lengths)/sizeof(string_lengths[0])); ++i)
        strcat(placeholder_line, " ");

    temp_logo[2] = placeholder_line;
    temp_logo[line_count] = "";

    puts("HELLO");

    dest = temp_logo;
}
*/

int main(const int argc, const char **argv) {
    bool user_is_an_idiot = false; // rtfm and stfu

    // are the following command line args used?
    bool asking_help = false;
    int asking_color = 0;
    int asking_bold = 0;
    int asking_logo = 0;
    int asking_align = 0;

    bool using_custom_config = false;

    // the config that's normally used is ~/.config/albafetch.conf
    char config_file[LOGIN_NAME_MAX + 33] = "";

    // parsing the command args
    for(int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            asking_help = true;
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color"))
            asking_color = i+1;
        else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold"))
            asking_bold = i+1;
        else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo"))
            asking_logo = i+1;
        else if(!strcmp(argv[i], "--align") || !strcmp(argv[i], "-a"))
            asking_align = i+1;
        else if(!strcmp(argv[i], "--config")) {
            if(i+1 < argc) {
                using_custom_config = true;
                strcpy(config_file, argv[i+1]);
                continue;
            }
            fputs("\e[31m\e[1mERROR\e[0m: --config requires a extra argument! Use --help for more info\n", stderr);
            user_is_an_idiot = true;
        }
    }

    if(!using_custom_config) {
        char *config_home = getenv("XDG_CONFIG_HOME");
        if(config_home) { // is XDG_CONFIG_HOME set?
            if(config_home[0]) // is XDG_CONFIG_HOME empty?
                snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/albafetch.conf", config_home);
            else
                snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", getenv("HOME"));
        }
        else
            snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", getenv("HOME"));
    }

    parse_config(config_file, using_custom_config);
    if(!(*spacing_first)) strcpy(spacing_first, spacing);
    if(!(*spacing_last)) strcpy(spacing_last, spacing);

    //logo_from_string("line1\nline2\nline3\nline4\nline5\nline6\n", logo);

    if(asking_color) {
        if(asking_color < argc) {
            char *colors[9][2] = {
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
                if(!strcmp(argv[asking_color], *colors[j])) {
                    strcpy(config.color, colors[j][1]);
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
    }

    if(asking_logo) {
        if(asking_logo < argc) {
            for(int j = 0; j < sizeof(logos)/sizeof(*logos); ++j)
                if(!strcmp(logos[j][0], argv[asking_logo])) {
                    logo = (char**)logos[j];
                    goto logo_arg_found;
                }
            logo = (char**)logos[0];
            logo_arg_found: ;
        } else {
            fputs("\e[31m\e[1mERROR\e[0m: --logo requires a extra argument! Use --help for more info\n", stderr);
            user_is_an_idiot = true;
        }
    }

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
            read_after_sequence(fp, "\nID", os_id, 32);
            if(!(*os_id))
                read_after_sequence(fp, "ID", os_id, 32);
            fclose(fp);

            char *end = strchr(os_id, '\n');
            if(!end)
                return -1;
            *end = 0;

            for(int i = 0; i < sizeof(logos)/sizeof(*logos); ++i)
                if(!strcmp(logos[i][0], os_id)) {
                    logo = (char**)logos[i];
                    goto logo_found;
                }
            logo = (char**)logos[0];
        #endif
        
        logo_found: ;
    }
    if(!(*config.color))
        strcpy(config.color, logo[1]);

    if(asking_help) {
        printf("%s%salbafetch\e[0m - a system fetch utility\n",
               config.color, config.bold);

        printf("\n%s%sFLAGS\e[0m:\n",
               config.color, config.bold);

        printf("\t%s%s-h\e[0m,%s%s --help\e[0m:\t Print this help menu and exit\n",
               config.color, config.bold, config.color, config.bold);

        printf("\t%s%s-c\e[0m,%s%s --color\e[0m:\t Change the output color (%s%s\e[0m)\n"
               "\t\t\t   [\e[30mblack\e[0m, \e[31mred\e[0m, \e[32mgreen\e[0m, \e[33myellow\e[0m,"
               " \e[34mblue\e[0m, \e[35mpurple\e[0m, \e[36mcyan\e[0m, \e[90mgray\e[0m,"
               " \e[37mwhite\e[0m]\n",
               config.color, config.bold, config.color, config.bold, default_color[0] ? default_color : logo[1], default_color[0] ? "default" : "logo default");

        printf("\t%s%s-b\e[0m,%s%s --bold\e[0m:\t Specifies if bold should be used in colored parts (default: %s\e[0m)\n"
               "\t\t\t   [\e[1mon\e[0m, off]\n",
               config.color, config.bold, config.color, config.bold, default_bold ? "\e[1mon" : "off");
        
        printf("\t%s%s-l\e[0m,%s%s --logo\e[0m:\t Changes the logo that will be displayed (%s)\n"
               "\t\t\t   [linux, apple, arch, arch_small, debian, linuxmint, endeavouros, ubuntu]\n"
               "\t\t\t   [parrot, manjaro, fedora, neon, pop, gentoo, windows]\n",
               config.color, config.bold, config.color,config. bold, default_logo[0] ? default_logo : "OS default");

        printf("\t%s%s-a\e[0m, %s%s--align\e[0m:\t Alignes the infos if set (default: %s)\n"
               "\t\t\t   [on, off]\n", config.color, config.bold, config.color, config.bold, config.align_infos ? "on" : "off");

        printf("\t%s%s--config\e[0m:\t Specifies a custom config (default: ~/.config/albafetch.conf)\n"
               "\t\t\t   [path]\n", config.color, config.bold);

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\n",
               config.color, config.bold);

        return 0;
    }

    if(asking_align) {
        if(asking_logo < argc) {
                if(!strcmp(argv[asking_align], "on"))
                    config.align_infos = true;
                else if(!strcmp(argv[asking_align], "off"))
                    config.align_infos = false;
                else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid extra argument for --align! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
        } else {
            fputs("\e[31m\e[1mERROR\e[0m: --align requires a extra argument! Use --help for more info\n", stderr);
            user_is_an_idiot = true;
        }
    }

    if(asking_bold) {
        if(asking_bold < argc) {
            if(!strcmp(argv[asking_bold], "on"))
                strcpy(config.bold, "\e[1m");
            else if(!strcmp(argv[asking_bold], "off"))
                strcpy(config.bold, "");
            else {
                fputs("\e[31m\e[1mERROR\e[0m: invalid extra argument for --bold! Use --help for more info\n", stderr);
                user_is_an_idiot = true;
            }
        } else {
            fputs("\e[31m\e[1mERROR\e[0m: --bold requires a extra argument! Use --help for more info\n", stderr);
            user_is_an_idiot = true;
        }
    }

    // die if the user is a fucking moron
    if(user_is_an_idiot)
        return 1;

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
        separator2,
        colors,
        light_colors
    };

    // The following line works because infos is declared on the stack,
    // so sizeof returns it's real size and not the size of a pointer.
    const int info_lines = (int)(sizeof(infos) / sizeof(*infos)) - 1;

    int line = 3;   // keeps track of the line that's currently printing 

    // logo and infos
    for(int i = 0; i <= info_lines; ++i) {
        line = printLogo(line);
        if(line == 4) printf("%s", spacing_first);
        else if(i == info_lines) printf("%s", spacing_last);
        else printf("%s", spacing);
        infos[i]();
        printf("\n");
    }

    // ******** remaining lines of the logo ********
    while(*logo[line]) {
        line = printLogo(line);
        printf("\n");
    }
    printf("\e[0m");
    fflush(stdout);

    return 0;
}
