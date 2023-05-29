#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include "info.h"
#include "utils.h"
#include "logos.h"
#include "queue.h"

/* TODO:
 * print de, shell and terminal versions
 * print kernel type (e.g. "Kernel: 6.3.2 (zen)") when using kernel_short
 * Windows support? *BSD support?
 */

// This contains the default config values
struct Config config = {
    // Default values for boolean options (least to most significant bit)
    // 1001 1111 0111 1111 1000 1110
    0x9f7f8e,

    NULL,   // logo
    "",     // color
    ": ",   // dash
    "-",    // separator
    5,      // spacing

    "%02d/%02d/%d %02d:%02d:%02d",  // date_format
    3,      // col_block_len

    "",         // separator_prefix
    "",         // spacing_prefix
    "",         // title_prefix
    "User",     // user_prefix
    "Hostname", // hostname_prefix
    "Uptime",   // uptime_prefix
    "OS",       // os_prefix
    "Kernel",   // kernel_prefix
    "Desktop",  // desktop_prefix
    "Shell",    // shell_prefix
    "Login",    // login_shell_prefix
    "Terminal", // term_prefix
    "Packages", // pkg_prefix
    "Host",     // host_prefix
    "BIOS",     // bios_prefix
    "CPU",      // cpu_prefix
    "GPU",      // gpu_prefix
    "Memory",   // mem_prefix
    "Public IP",// pub_prefix
    "Local IP", // loc_prefix
    "Directory",// pwd_prefix
    "Date",     // date_prefix
    "",         // colors_prefix
    "",         // light_colors_prefix
};



int main(int argc, char **argv) {
    bool user_is_an_idiot = false; // rtfm and stfu

    // are the following command line args used?
    bool asking_help = false;
    bool use_config = true;
    int asking_color = 0;
    int asking_bold = 0;
    int asking_logo = 0;
    int asking_align = 0;

    // these store either the default values or the ones defined in the config
    // they are needed to know what is used if no arguments are given (for --help)
    bool default_bold = bold;
    char default_color[8] = "";
    char default_logo[16] = "";

    // the default config file is ~/.config/albafetch.conf
    char config_file[LOGIN_NAME_MAX + 64] = "";
    // path of a custom ascii art
    char *ascii_file = NULL;

    // parsing the command args
    for(int i = 1; i < argc; ++i) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            asking_help = true;
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color"))
            asking_color = i+1;
        else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold"))
            asking_bold = i+1;
        else if(!strcmp(argv[i], "--ascii")) {
            if(i+1 >= argc || use_config == false) {   // is there such an arg?
                fputs("\033[31m\033[1mERROR\033[0m: --config requires an extra argument!\n", stderr);
                user_is_an_idiot = true;
                continue;
            }
            else if(access(argv[i+1], F_OK)) {  // is it a valid file?
                fprintf(stderr, "\033[31m\033[1mERROR\033[0m: invalid file \"%s\"! Use --help for more info\n", argv[i+1]);
                user_is_an_idiot = true;
                continue;
            }
            ascii_file = argv[i+1];
            continue;
        }
        else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo"))
            asking_logo = i+1;
        else if(!strcmp(argv[i], "--align") || !strcmp(argv[i], "-a"))
            asking_align = i+1;
        else if(!strcmp(argv[i], "--config") && use_config) {
            if(i+1 >= argc || use_config == false) {   // is there such an arg?
                fputs("\033[31m\033[1mERROR\033[0m: --config requires an extra argument!\n", stderr);
                user_is_an_idiot = true;
                continue;
            }
            else if(access(argv[i+1], F_OK)) {  // is it a valid file?
                fprintf(stderr, "\033[31m\033[1mERROR\033[0m: invalid file \"%s\"! Use --help for more info\n", argv[i+1]);
                user_is_an_idiot = true;
                continue;
            }
            strncpy(config_file, argv[i+1], sizeof(config_file)-1);
            continue;
        }
        else if(!strcmp(argv[i], "--no-config"))
            use_config = false;
    }

    char *mem = mmap(NULL, 8192, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memset(mem, 0, 8192);
    /* this chunk is divided as following (every line represents 256B):
     * 0  
     * 1  
     * 2  printed       (what should get printed, used for parsing)
     * 3  printed
     * 4  printed
     * 5  
     * 6  data          (buffer for every function in info.c)
     * 7  
     * 8  logo          (when using a custom ascii, used to store
     * 10 logo           some logo metadata (id, color, length)
     * 11 logo           and the lines of the logo).
     * .. logo
     * 31 logo
     */
    
    struct Module *modules = malloc(sizeof(struct Module));
    modules->id = NULL;
    modules->next = NULL;

    // albafetch will first parse ~/.config/albafetch.conf
    // ~/.config/albafetch/albafetch.conf if the former is not found
    if(use_config) {    // --no-config was not used
        if(!config_file[0]) {   // --config was not used, using the default path
            char *home = getenv("HOME");
            char *config_home = getenv("XDG_CONFIG_HOME");

            if(config_home) { // is XDG_CONFIG_HOME set?
                snprintf(config_file, sizeof(config_file), "%s/albafetch.conf", config_home);
                if(access(config_file, F_OK))
                    snprintf(config_file, sizeof(config_file), "%s/albafetch/albafetch.conf", config_home);
            }
            else if(home) {  // is HOME set?
                snprintf(config_file, sizeof(config_file), "%s/.config/albafetch.conf", home);
                if(access(config_file, F_OK))
                    snprintf(config_file, sizeof(config_file), "%s/.config/albafetch/albafetch.conf", home);
            }
            else    // WHY TF WOULD HOME NOT BE SET???
                return -1;
        }

        parse_config(config_file, modules, mem, &default_bold, default_color, default_logo);
    }

    //char *logo[68];
    if(ascii_file)
        file_to_logo(ascii_file, mem + 2048);

    if(asking_logo) {   // --logo was used
        bool found = false;
        if(asking_logo < argc) {
            // find the matching logo
            for(size_t i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
                if(!strcmp(logos[i][0], argv[asking_logo])) {
                    config.logo = (char**)logos[i];
                    found = true;
                }

            if(!found)
                fprintf(stderr, "\033[31m\033[1mERROR\033[0m: invalid logo \"%s\"! Use --help for more info\n", argv[asking_logo]);
        } else
            fputs("\033[31m\033[1mERROR\033[0m: --logo requires an extra argument!\n", stderr);

        if(found)
            strcpy(config.color, config.logo[1]);
        else
            user_is_an_idiot = true;
    }
    if(!config.logo) {  // get a logo based on the OS (--logo was not used and no logo was set by the config)
        #ifdef __APPLE__
            config.logo = (char**)logos[1];
        #else
        # ifdef __ANDROID__
            config.logo = (char**)logos[2];
        # else
            config.logo = (char**)logos[0];
            FILE *fp = fopen("/etc/os-release", "r");

            if(!fp)
                fp = fopen("/usr/lib/os-release", "r");

            if(fp) {
                char os_id[48];
                read_after_sequence(fp, "\nID", os_id, 48);
                if(!os_id[0])
                    read_after_sequence(fp, "ID", os_id, 48);
                fclose(fp);

                char *end = strchr(os_id, '\n');
                if(end)
                    *end = 0;

                // find the matching logo
                for(size_t i = 0; i < sizeof(logos)/sizeof(*logos); ++i)
                    if(!strcmp(logos[i][0], os_id)) {
                        config.logo = (char**)logos[i];
                        break;
                    }
            }
        # endif // __ANDROID__
        #endif // __APPLE__
        
        strcpy(default_logo, config.logo[0]);
        strcpy(config.color, config.logo[1]);
    }

    if(asking_color) {
        if(asking_color < argc) {
            char *colors[9][2] = {
                {"black", "\033[30m"},
                {"red", "\033[31m"},
                {"green", "\033[32m"},
                {"yellow", "\033[33m"},
                {"blue", "\033[34m"},
                {"purple", "\033[35m"},
                {"cyan", "\033[36m"},
                {"gray", "\033[90m"},
                {"white", "\033[37m"},
            };

            for(int j = 0; j < 9; ++j)
                if(!strcmp(argv[asking_color], *colors[j])) {
                    strcpy(config.color, colors[j][1]);
                    goto color_done;
                }

            fprintf(stderr, "\033[31m\033[1mERROR\033[0m: invalid color \"%s\"! Use --help for more info\n", argv[asking_color]);
        }
        else
            fputs("\033[31m\033[1mERROR\033[0m: --color requires an extra argument!\n", stderr);

        user_is_an_idiot = true;

        color_done:;
    }

    if(asking_bold) {
        if(asking_bold < argc) {
            // modifying the 2nd least significant bit of options
            if(!strcmp(argv[asking_bold], "on")) {
                config.options |= ((uint64_t)1 << 1);
                goto bold_done;
            }
            else if(!strcmp(argv[asking_bold], "off")) {
                config.options &= ~((uint64_t)1 << 1);
                goto bold_done;
            }

            fputs("\033[31m\033[1mERROR\033[0m: --bold should be followed by either \"on\" or \"off\"!\n", stderr);
        }
        else
            fputs("\033[31m\033[1mERROR\033[0m: --bold requires an extra argument!\n", stderr);

        user_is_an_idiot = true;

        bold_done:;
    }

    // was it really that hard to type 'albafetch -h'?
    if(user_is_an_idiot) {
        destroy_array(modules);

        fputs("\033[31m\033[1mFATAL\033[0m: One or multiple errors occurred! Use --help for more info\n", stderr);

        return 1;
    }

    if(asking_help) {
        // it won't be used anyway lol
        destroy_array(modules);

        printf("%s%salbafetch\033[0m - a system fetch utility\n",
               config.color, bold ? "\033[1m" : "");

        printf("\n%s%sFLAGS\033[0m:\n",
               config.color, bold ? "\033[1m" : "");

        printf("\t%s%s-h\033[0m,%s%s --help\033[0m:\t Print this help menu and exit\n",
               config.color, bold ? "\033[1m" : "", config.color, bold ? "\033[1m" : "");

        printf("\t%s%s-c\033[0m,%s%s --color\033[0m:\t Change the output color (%s%s\033[0m)\n"
               "\t\t\t   [\033[30mblack\033[0m, \033[31mred\033[0m, \033[32mgreen\033[0m, \033[33myellow\033[0m,"
               " \033[34mblue\033[0m, \033[35mpurple\033[0m, \033[36mcyan\033[0m, \033[90mgray\033[0m,"
               " \033[37mwhite\033[0m]\n",
               config.color, bold ? "\033[1m" : "", config.color, bold ? "\033[1m" : "", default_color[0] ? default_color : config.logo[1], default_color[0] ? "default" : "logo default");

        printf("\t%s%s-b\033[0m,%s%s --bold\033[0m:\t Specifies if bold should be used in colored parts (default: %s\033[0m)\n"
               "\t\t\t   [\033[1mon\033[0m, off]\n",
               config.color, bold ? "\033[1m" : "", config.color, bold ? "\033[1m" : "", default_bold ? "\033[1mon" : "off");
        
        printf("\t%s%s-l\033[0m,%s%s --logo\033[0m:\t Changes the logo that will be displayed (default: %s)\n"
               "\t\t\t   [android, apple, arch, arch_small, debian, endeavouros, fedora]\n"
               "\t\t\t   [gentoo, linux, linuxmint, manjaro, neon, parrot, pop, ubuntu, windows]\n",
               config.color, bold ? "\033[1m" : "", config.color, bold ? "\033[1m" : "", default_logo[0] ? default_logo : "OS Default");

        printf("\t%s%s--ascii\033[0m:\t Specifies a file containing a custom ascii art to use as logo (default: none)\n"
               "\t\t\t   [path]\n", config.color, bold ? "\033[1m" : "");

        printf("\t%s%s-a\033[0m, %s%s--align\033[0m:\t Aligns the infos if set (default: %s)\n"
               "\t\t\t   [on, off]\n", config.color, bold ? "\033[1m" : "", config.color, bold ? "\033[1m" : "", align ? "on" : "off");

        printf("\t%s%s--config\033[0m:\t Specifies a custom config (default: ~/.config/albafetch.conf)\n"
               "\t\t\t   [path]\n", config.color, bold ? "\033[1m" : "");

        printf("\t%s%s--no-config\033[0m:\t Ignores any provided or existing config file\n",
               config.color, bold ? "\033[1m" : "");

        printf("\nReport a bug: %s%s\033[4mhttps://github.com/alba4k/albafetch/issues\033[0m\n",
               config.color, bold ? "\033[1m" : "");

        return 0;
    }

    if(asking_align) {
        if(asking_align < argc) {
            if(!strcmp(argv[asking_align], "on")) {
                config.options |= (uint64_t)1;
                goto align_done;
            }
            else if(!strcmp(argv[asking_align], "off")) {
                config.options &= ~((uint64_t)1);
                goto align_done;
            }

            fputs("\033[31m\033[1mERROR\033[0m: --align should be followed by either \"on\" or \"off\"!\n", stderr);
        }
        else
            fputs("\033[31m\033[1mERROR\033[0m: --align requires an extra argument!\n", stderr);

        user_is_an_idiot = true;

        align_done:;
    }

    // I am deeply sorry for the code you're about to see - I hope you like spaghetti
    unsigned line = 2;
    char *data = mem + 1536;
    char *printed = mem+512;
    char format[32] = "%s\033[0m%s";

    /* getting the terminal width
     * I start by using stdout
     * if it is not a terminal (e.g. if the user did albafetch | lolcat) I use stderr
     * if stderr doesn't work either, I use stdin (albafetch 2>/dev/null | lolcat)"
     * now, let's assume the user is a complete moron and redirects everything. Then I just use an "infinite" width
     */
    struct winsize w;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        if(ioctl(STDERR_FILENO, TIOCGWINSZ, &w) == -1)
            if(ioctl(STDIN_FILENO, TIOCGWINSZ, &w) == -1)
                w.ws_col = -1;

    struct Info {
        char *id;               // module identifier
        char *label;            // module label
        int (*func)(char *);    // function to run
    };
    struct Info module_table[] = {
     // {"identifier", "label", func},
        {"separator", config.separator_prefix, NULL},
        {"space", config.spacing_prefix, NULL},
        {"title", config.title_prefix, NULL},
        {"user", config.user_prefix, user},
        {"hostname", config.hostname_prefix, hostname},
        {"uptime", config.uptime_prefix, uptime},
        {"os", config.os_prefix, os},
        {"kernel", config.kernel_prefix, kernel},
        {"desktop", config.desktop_prefix, desktop},
        {"shell", config.shell_prefix, shell},
        {"login_shell", config.login_shell_prefix, login_shell},
        {"term", config.term_prefix, term},
        {"packages", config.pkg_prefix, packages},
        {"host", config.host_prefix, host},
        {"bios", config.bios_prefix, bios},
        {"cpu", config.cpu_prefix, cpu},
        {"gpu", config.gpu_prefix, gpu},
        {"memory", config.mem_prefix, memory},
        {"public_ip", config.pub_prefix, public_ip},
        {"local_ip", config.loc_prefix, local_ip},
        {"pwd", config.pwd_prefix, pwd},
        {"date", config.date_prefix, date},
        {"colors", config.colors_prefix, colors},
        {"light_colors", config.light_colors_prefix, light_colors},
    };

    // this sets the default module order in case it was not set in a config file
    if(!modules->next) {
        char *default_order[] = {
            "title",
            "separator",
            "uptime",
            "separator",
            "os",
            "kernel",
            "desktop",
            "shell",
            "term",
            "packages",
            "separator",
            "host",
            "cpu",
            "gpu",
            "memory",
            "space",
            "colors",
            "light_colors",
        };

        for(size_t i = 0; i < sizeof(default_order)/sizeof(default_order[0]); ++i)
            add_module(modules, default_order[i]);
    }

    // filling in the linked list (with function pointers and labels) based on the ids
    // I prefer doing it here than in add_module as this part only runs when it's needed
    for(struct Module *current = modules->next; current; current = current->next)
        for(size_t i = 0; i < sizeof(module_table)/sizeof(module_table[0]); ++i)
            if(!strcmp(module_table[i].id, current->id)) {
                current->label = module_table[i].label;
                current->func = module_table[i].func;
            }

    if(align) {
        int current_len;
        asking_align = 0;

        // determining how far the text should be aligned
        for(struct Module *current = modules->next; current; current = current->next) {
            current_len = strlen_real(current->label);

            if(current_len > asking_align)
                asking_align = current_len;
        }

        asking_align += strlen_real(config.dash);

        snprintf(format, 32, "%%-%ds\033[0m%%s", asking_align);
    }
    
    // printing every module
    for(struct Module *current = modules->next; current; current = current->next) {
        if(!strcmp(current->id, "separator")) {    // separators are handled differently
            if(!(printed[0]) || strlen(printed) > 767)
                continue;

            // this is the length of the previous printed text
            size_t len = (strlen_real(printed
                                     + strlen(config.logo[line] ? config.logo[line] : config.logo[2])
                                     + config.spacing
                                    ) - strlen_real(current->label))
                                    / strlen_real(config.separator);

            printed[0] = 0;

            get_logo_line(printed, &line);

            for(int i = 0; i < config.spacing; ++i)
                strcat(printed, " ");

            strcat(printed, config.color);
            strcat(printed, current->label);

            for(size_t i = 0; i < len && strlen(printed) < 767 - strlen(config.separator); ++i)
                strcat(printed, config.separator);
        }
        else if(!strcmp(current->id, "space")) {  // spacings are handled differently (they don't do shit)
            printed[0] = 0;

            get_logo_line(printed, &line);

            for(int i = 0; i < config.spacing; ++i)
                strcat(printed, " ");

            strcat(printed, config.color);
            strcat(printed, current->label);
        }
        else if(!strcmp(current->id, "title")) {    // titles are handled differently
            char name[256];
            char host[256];

            if(user(name) || hostname(host))
                continue;

            printed[0] = 0;

            get_logo_line(printed, &line);

            for(int i = 0; i < config.spacing; ++i)
                strcat(printed, " ");

            strcat(printed, config.color);
            strcat(printed, current->label);

            if(title_color)
                snprintf(printed+strlen(printed), 768-strlen(printed), "%s%s%s%s@%s%s%s",
                    config.color,
                    bold ? "\033[1m" : "",
                    name,
                    "\033[0m",
                    bold ? "\033[1m" : "",
                    config.color,
                    host
                );
            else
                snprintf(printed+strlen(printed), 768-strlen(printed), "%s%s@%s",
                    "\033[0m",
                    name,
                    host
                );
        }
        else if(current->func == NULL) {            // printing a custom text
            printed[0] = 0;

            get_logo_line(printed, &line);

            for(int i = 0; i < config.spacing; ++i)
                strcat(printed, " ");

            strcat(printed, config.color);
            strcat(printed, current->id);
        }
        else {
            if((current->func)(data))
                continue;

            char label[80];
            printed[0] = 0;

            get_logo_line(printed, &line);

            for(int i = 0; i < config.spacing; ++i)
                strcat(printed, " ");
                
            strcat(printed, config.color);
            strcpy(label, current->label);
            if(current->label[0] && current->func != colors && current->func != light_colors)
                strcat(label, config.dash);

            snprintf(printed+strlen(printed), 768-strlen(printed), format, label, data);
        }
        
        print_line(printed, w.ws_col);
    }

    // remaining lines
    while(config.logo[line+1]) {
        printed[0] = 0;

        get_logo_line(printed, &line);

        print_line(printed, w.ws_col);
    }

    destroy_array(modules);
    return 0;
}
