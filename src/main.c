#include <stdio.h>

#include <sys/mman.h>

#include "info.h"
#include "utils.h"
#include "logos.h"
#include "queue.h"

#ifndef MAP_ANONYMOUS   // https://github.com/microsoft/vscode-cpptools/issues/4268
#define MAP_ANONYMOUS 0x20
#endif

/* TODO:
 * use strlen() to determine how far to --align ("%-%ds", max(strlen(a), strlen(b)) + 2 <-- NOT actual code)
 * !!! option to choose what order the infos are printed in ( modules {"a", "b"} in albafetch.conf)
 * --ascii for custom ascii art (conflicts with --logo) - work in progress (lines [64; 76]U[318; 356])
 * remove the lspci dependency for gpu()
 * more config options (e.g. complete login shell path)
 */

Config config = {
    true,  // print_shell_path
    false, // show_localdomain
};

int main(int argc, char **argv) {
    bool user_is_an_idiot = false; // rtfm and stfu

    // are the following command line args used?
    bool asking_help = false;
    int asking_color = 0;
    int asking_bold = 0;
    int asking_logo = 0;
    int asking_align = 0;

    bool using_custom_config = false;

    char *memory = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    // the config that's normally used is ~/.config/albafetch.conf
    char config_file[LOGIN_NAME_MAX + 33] = "";

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

    // DEBUG FOR SINGLE FUNCTIONS
    char *data = memory + 1024;

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

    if(!host(data))
        printf("%s\n", data);

    if(!bios(data))
        printf("%s\n", data);

    if(!gpu(data))
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
}
