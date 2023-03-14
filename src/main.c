#include <stdio.h>

#include <sys/mman.h>

#include "info.h"
#include "utils.h"
#include "logos.h"
#include "queue.h"

#ifndef MAP_ANONYMOUS   // https://github.com/microsoft/vscode-cpptools/issues/4268 vscode being annoying
#define MAP_ANONYMOUS 0x20
#endif

/* TODO:
 * !!! option to choose what order the infos are printed in ( modules {"a", "b"} in albafetch.conf)
 * --ascii for custom ascii art (conflicts with --logo) - work in progress
 * more config options (for the single functions)
 */

// various configurations for stuff
struct Config config = {
    true,   // title_color
    true,   // os_arch
    false,  // de_type
    false,  // shell_path
    true,   // cpu_brand
    true,   // cpu_freq
    true,   // cpu_count
    false,  // gpu_brand
    true,   // mem_perc
    true,   // pkg_mgr
    true,   // pkg_pacman
    true,   // pkg_dpkg
    true,   // pkg_rpm
    true,   // pkg_flatpak
    true,   // pkg_snap
    false,  // pkg_pip
    true,   // pkg_brew
    true,   // loc_localdomain

    false,  // align
    "",     // color
};

int main(int argc, char **argv) {
    // using a linked list for this is quite horrible, but here we go
    struct Info infos[64] = {
    //  {"Label", fptr, next}
        {NULL, NULL, infos+3},              // 00
        {"User", user, NULL},               // 01
        {"Hostname", hostname, NULL},       // 02
        {"", title, infos+23},              // 03
        {"Uptime", uptime, infos+24},       // 04
        {"OS", os, infos+6},                // 05
        {"Kernel", kernel, infos+7},        // 06
        {"Desktop", desktop, infos+8},      // 07
        {"Shell", shell, infos+10},         // 08
        {"Login", login_shell, NULL},       // 09
        {"Terminal", term, infos+11},       // 10
        {"Packages", packages, infos+25},   // 11
        {"Host", host, infos+14},           // 12
        {"BIOS", bios, NULL},               // 13
        {"CPU", cpu, infos+15},             // 14
        {"GPU", gpu, infos+16},             // 15
        {"Memory", memory, infos+26},       // 16
        {"Public IP", public_ip, NULL},     // 17
        {"Local IP", local_ip, NULL},       // 18
        {"Directory", pwd, NULL},           // 19
        {"Date", date, NULL},               // 20
        {"", colors, infos+22},             // 21
        {"", light_colors, NULL},           // 22
        {"", separator, infos+4},           // 23
        {"", separator, infos+5},           // 24
        {"", separator, infos+12},          // 25
        {"", separator, infos+21},          // 26
    };

    bool user_is_an_idiot = false; // rtfm and stfu

    // are the following command line args used?
    bool asking_help = false;
    int asking_color = 0;
    int asking_bold = 0;
    int asking_logo = 0;
    int asking_align = 0;

    bool using_custom_config = false;

    char *mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memset(mem, 0, 4096);
    /* this chunk is divided as following (one line is 256B):
     * 0  
     * 1   
     * 2  printed       (saves what should get printed, used for parsing)
     * 3  printed       (saves what should get printed, used for parsing)
     * 4  data          (buffer for every function in info.c)
     * 5  
     * 6  
     * 7  
     * 8  logo          (32 char* to lines (following logo.h))
     * 9  
     * 10 
     * 11 
     * 12 
     * 13 
     * 14 
     * 15 
     */

    // the config that's normally used is ~/.config/albafetch.conf
    char config_file[LOGIN_NAME_MAX + 32] = "";

    char *home = getenv("HOME");
    // I really hope this part will never run
    if(!home) {
        fputs("\e[31m\e[1mERROR\e[0m: $HOME is not set, interrupting!\n", stderr);

        return -1;
    }
    if(!home[0]) {
        fputs("\e[31m\e[1mERROR\e[0m: $HOME is empty, interrupting!\n", stderr);
        return -1;
    }

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
            fputs("\e[31m\e[1mERROR\e[0m: --config requires an extra argument!\n", stderr);
            user_is_an_idiot = true;
        }
    }
    
    char **logo = (char**)logos[0];
    #ifdef __APPLE__
        logo = (char**)logos[1];
    #else
        FILE *fp = fopen("/etc/os-release", "r");

        if(fp) {
            char os_id[48];
            read_after_sequence(fp, "\nID", os_id, 48);
            if(!os_id[0])
                read_after_sequence(fp, "ID", os_id, 48);
            fclose(fp);

            char *end = strchr(os_id, '\n');
            if(!end)
                return -1;
            *end = 0;

            for(size_t i = 0; i < sizeof(logos)/sizeof(*logos); ++i)
                if(!strcmp(logos[i][0], os_id))
                    logo = (char**)logos[i];
        }
    #endif
    strcpy(config.color, logo[1]);

    if(!using_custom_config) {
        char *config_home = getenv("XDG_CONFIG_HOME");
        if(config_home) { // is XDG_CONFIG_HOME set?
            if(config_home[0]) // is XDG_CONFIG_HOME empty?
                snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/albafetch.conf", config_home);
            else
                snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", home);
        }
        else
            snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", home);
    }

    // TODO: logo
    if(asking_logo) {
        if(asking_logo < argc) {
            bool done = false;

            for(size_t i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
                if(!strcmp(logos[i][0], argv[asking_logo])) {
                    logo = (char**)logos[i];
                    done = true;
                }

            if(!done) {
                fprintf(stderr, "\e[31m\e[1mERROR\e[0m: invalid logo \"%s\"! Use --help for more info\n", argv[asking_logo]);
                user_is_an_idiot = true;
            }
        } else {
            fputs("\e[31m\e[1mERROR\e[0m: --logo srequires an extra argument!\n", stderr);
            user_is_an_idiot = true;
        }

        strcpy(config.color, logo[1]);
    }

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

            bool done = false;
            for(int j = 0; j < 9; ++j)
                if(!strcmp(argv[asking_color], *colors[j])) {
                    strcpy(config.color, colors[j][1]);
                    done = true;
                }

            if(!done) {
                fprintf(stderr, "\e[31m\e[1mERROR\e[0m: invalid color \"%s\"! Use --help for more info\n", argv[asking_color]);
                user_is_an_idiot = true;
            }
        }
        else {
            fputs("\e[31m\e[1mERROR\e[0m: --color srequires an extra argument!\n", stderr);
            user_is_an_idiot = true;
        }
    }

    // TODO: bold
    if(asking_bold) {
        if(asking_bold < argc) {
            if(!strcmp(argv[asking_bold], "on"))
                ; // PLACEHOLDER
            else if(!strcmp(argv[asking_bold], "off"))
                ; // PLACEHOLDER
            else {
                fputs("\e[31m\e[1mERROR\e[0m: --bold should be followed by either \"on\" or \"off\"!\n", stderr);
                user_is_an_idiot = true;
            }
        }
        else {
            fputs("\e[31m\e[1mERROR\e[0m: --bold srequires an extra argument!\n", stderr);
            user_is_an_idiot = true;
        }
    }

    if(asking_align) {
        if(asking_align < argc) {
            if(!strcmp(argv[asking_align], "on"))
                config.align = true;
            else if(!strcmp(argv[asking_align], "off"))
                config.align = false;
            else {
                fputs("\e[31m\e[1mERROR\e[0m: --align should be followed by either \"on\" or \"off\"!\n", stderr);
                user_is_an_idiot = true;
            }
        }
        else {
            fputs("\e[31m\e[1mERROR\e[0m: --align srequires an extra argument!\n", stderr);
            user_is_an_idiot = true;
        }
    }

    // was it really that hard to type 'albafetch -h'?
    if(user_is_an_idiot) {
        fputs("\e[31m\e[1mFATAL\e[0m: One or multiple errors occured! Use --help for more info\n", stderr);
        return 1;
    }

    // TODO: help
    if(asking_help) {
        
    }

#ifdef _DEBUG
    // DEBUG FOR SINGLE FUNCTIONS
    char *data = mem + 1024;

    // Is there a smarter way to do this? Probably, but I'm too lazy
    // Feel free to let me know or just open a PR if you know one <3
    config.os_arch = 1;
    config.de_type = 1;
    config.shell_path = 1;
    config.cpu_brand = 1;
    config.cpu_freq = 1;
    config.cpu_count = 1;
    config.gpu_brand = 1;
    config.mem_perc = 1;
    config.pkg_mgr = 1;
    config.pkg_pacman = 1;
    config.pkg_dpkg = 1;
    config.pkg_rpm = 1;
    config.pkg_flatpak = 1;
    config.pkg_snap = 1;
    config.pkg_pip = 1;
    config.pkg_brew = 1;
    config.loc_localdomain = 1;

    int (*arr[])(char *) = {
        user,
        hostname,
        uptime,
        os,
        kernel,
        desktop,
        shell,
        login_shell,
        term,
        packages,
        host,
        bios,
        cpu,
        gpu,
        memory,
        public_ip,
        local_ip,
        pwd,
        date,
        colors,
        light_colors
    };

    for(unsigned long i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        if(!arr[i](data))
            printf("%s\n", data);
        else
            printf("%lu returned %d\n", i, arr[i](data));
    }
    return 0;
#else

    // I am deeply sorry for the code you're about to see - I hope you like spaghettis
    unsigned line = 3;
    char *data = mem + 1024;
    char *printed = mem+512;
    char format[32] = "%s\e[0m%s";

    if(config.align) {
        int nums[64];
        int i = 0;

        for(struct Info *current = (infos)->next; current; current = current->next) {
            nums[i] = strlen(current->label);
            ++i;
        }

        asking_align = max(nums, i) + strlen(": ");

        snprintf(format, 32, "%%-%ds\e[0m%%s", asking_align);
    }

    for(struct Info *current = infos->next; current; current = current->next) {
        if(current->func == separator && config.align) {    // the separators should not get aligned
            if(printed[0]) {
                print_line(logo, &line);
                
                for(size_t i = 0; i < strlen(printed) - 4; ++i)           // the separators should cover the aligment gap
                    fputs("-", stdout);
                puts("");
            }
        }
        else if(!((current->func)(data))) {
            print_line(logo, &line);

            size_t len = strlen(current->label) + strlen(": ");
            char label[len];

            strcpy(label, current->label);
            if(current->label[0])
                strcat(label, ": ");

            snprintf(printed, 512, format, label, data);
            if(!config.align)
                strncpy(data, printed, 256);
            printf("%s\n", printed);
        }
    }

    // remaining lines
    while(logo[line]) {
        print_line(logo, &line);
        printf("\n");
    }
    printf("\e[0m");

#endif // _DEBUG

    return 0;
}
