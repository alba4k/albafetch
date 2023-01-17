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
 * use strlen() to determine how far to --align ("%-%ds", max(strlen(a), strlen(b)) + 2 <-- NOT actual code)
 * !!! option to choose what order the infos are printed in ( modules {"a", "b"} in albafetch.conf)
 * --ascii for custom ascii art (conflicts with --logo) - work in progress (lines [64; 76]U[318; 356])
 * remove the lspci dependency for gpu()
 * more config options (e.g. complete login shell path)
 */

// various configurations for stuff
struct Config config = {
    true,   // shell_path
    true,   // show_localdomain
    true,   // cpu_brand
    true,   // cpu_freq
    true,   // mem_perc
    true,   // pkg_mgr
    true,   // pkg_pacman
    true,   // pkg_dpkg
    true,   // pkg_rpm
    true,   // pkg_flatpak
    true,   // pkg_snap
    false,  // pkg_pip
    true,   // pkg_brew

    false,  // align
};

int main(int argc, char **argv) {
    // using a linked list for this is horrible, but here we go
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

    // the config that's normally used is ~/.config/albafetch.conf
    char config_file[LOGIN_NAME_MAX + 32] = "";

    char *home = getenv("HOME");
    // I really hope this part will never need to run
    if(!home) {
        fflush(stdout);
        fputs("\e[31m\e[1mERROR\e[0m:$HOME is not set, interrupting!\n", stderr);
        fflush(stderr);

        return 1;
    }
    if(!home[0]) {
        fflush(stdout);
        fputs("\e[31m\e[1mERROR\e[0m:$HOME is empty, interrupting!\n", stderr);
        fflush(stderr);

        return 1;
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
                snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", home);
        }
        else
            snprintf(config_file, LOGIN_NAME_MAX + 32, "%s/.config/albafetch.conf", home);
    }

    if(asking_align) {
        if(argc > asking_align) {
            if(!strcmp(argv[asking_align], "on"))
                config.align = true;
            else if(!strcmp(argv[asking_align], "off"))
                config.align = false;
        }
        else {
            // TODO: error message or warning
            return 1;
        }
    }


#ifdef _DEBUG
    // DEBUG FOR SINGLE FUNCTIONS
    char *data = mem + 1024;

    if(!user(data))
        printf("%s\n", data);

    if(!hostname(data))
        printf("%s\n", data);

    if(!uptime(data))
        printf("%s\n", data);

    if(!os(data))
        printf("%s\n", data);

    if(!kernel(data))
        printf("%s\n", data);

    if(!desktop(data))
        printf("%s\n", data);

    if(!shell(data))
        printf("%s\n", data);

    if(!login_shell(data))
        printf("%s\n", data);

    if(!term(data))
        printf("%s\n", data);

    if(!packages(data))
        printf("%s\n", data);

    if(!host(data))
        printf("%s\n", data);

    if(!bios(data))
        printf("%s\n", data);

    if(!cpu(data))
        printf("%s\n", data);

    if(!gpu(data))
        printf("%s\n", data);

    if(!memory(data))
        printf("%s\n", data);

    if(!public_ip(data))
        printf("%s\n", data);

    if(!local_ip(data))
        printf("%s\n", data);

    if(!pwd(data))
        printf("%s\n", data);

    if(!date(data))
        printf("%s\n", data);

    if(!colors(data))
        printf("%s\n", data);

    if(!light_colors(data))
        printf("%s\n", data);

    // END
    return 0;
#else

    // I am deeply sorry for the code you're about to see - I hope you like spaghettis
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
            for(int i = 0; i < asking_align; ++i)           // the separators should cover the aligment gap
                strcat(data, "#");
                
            separator(data);
            printf("%s\n", data);
        }
        else if(!((current->func)(data))) {
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
#endif // _DEBUG

    return 0;
}
