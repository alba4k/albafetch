#include <stdio.h>
#include <unistd.h>             // STDOUT_FILENO
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>             // get max hostname lenght
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <libcpuid/libcpuid.h>  // http://libcpuid.sourceforge.net

#include "config.h"

bool user_is_an_idiot = false;

void separator() {      // prints a separator
    fputs(SEPARATOR, stdout);
}

void title(const char *color, const char * bold) {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s\e[0m@%s%s%s\e[0m", getlogin(), color, bold, hostname);
}

void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("Hostname:\e[0m %s", hostname);
}

void user() {           // get the current login
    printf("User:\e[0m %s", getlogin());
}

void uptime() {         // prints the uptime
    struct sysinfo info;
    sysinfo(&info);

    long secs = info.uptime;            // total uptime in seconds
    long days = secs/86400;
    long hours = secs/3600 - days*24;
    long mins = secs/60 - days*1440 - hours*60;
    long sec = secs - days*86400 - hours*3600 - mins*60;

    printf("%-11s\e[0m", "Uptime:");
    if(days) {
        printf("%ldd ", days);     // print the number of days passed if more than 0
    }
    if(hours) {
        printf("%ldh ", hours);       // print the number of days passed if more than 0
    }
    if(mins) {
        printf("%ldm ", mins);        // print the number of minutes passed if more than 0
    }
    else if(secs < 60) {
        printf("%lds", sec);         // print the number of seconds passed if more than 0
    }
}

void os() {             // prints the os name + arch
    struct utsname name;
    uname(&name);

    static char os_name[128];
    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "{ lsb_release -ds; } | tr -d '\"'", NULL);
    } else {
        printf("%-11s\e[0m", "OS:");
    }
    wait(NULL);

    close(pipes[1]);
    os_name[read(pipes[0], os_name, 128) - 1] = 0;

    close(pipes[0]);
    printf("%s %s", os_name, name.machine);
}

void kernel() {         // prints the kernel version
    struct utsname name;
    uname(&name);

    printf("%-11s\e[0m%s", "Kernel:" , name.release);
}

void desktop() {        // prints the current desktop environment
        printf("%-11s\e[0m%s", "Desktop:", getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP
}

void shell() {          // prints the user default shell
    printf("%-11s\e[0m%s", "Shell:", getenv("SHELL"));        // $SHELL
}

void term() {           // prints the current terminal
    printf("%-11s\e[0m%s", "Terminal:", getenv("TERM"));     // $TERM
}

void packages() {       // prints the number of installed packages
    // using pacman, only pacman, and I'll get away with it

    char packages[10];

    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "pacman -Q | wc -l", NULL);        // using "pacman --query" to list the installed packages; using "wc --lines" to get the number of lines (wordcount)
    } else {
        printf("%-11s\e[0m", "Packages:");
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 10) - 1] = 0;

    close(pipes[0]);
    printf("%s (pacman)", packages);
}

void host() {           // prints the current host machine
    printf("%-11s\e[0m%s", "Host:", HOST);
}

void cpu() {            // prints the current CPU
    // could get it from /proc/cpuinfo
    printf("%-11s\e[0m", "CPU:");

    struct cpu_raw_data_t raw; 
    struct cpu_id_t data;  

    if (!cpuid_present()) {                     // check for CPUID presence
		printf("no cpuid");
	}
    if (cpuid_get_raw_data(&raw) < 0) {         // obtain the raw CPUID data
		printf("%s", cpuid_error());            // cpuid_error() gives the last error description
	}
    if (cpu_identify(&raw, &data) < 0) {        // identify the CPU, using the given raw data.
		printf("%s", cpuid_error());
	}
    char *cpuname = data.brand_str;
    /*
    for(int i = 0; cpuname[i] != 0; i++) {
        if(!PRINT_CPU_FREQ && cpuname[i] == ' ' && cpuname[i+1] == 'C' && cpuname[i+2] == 'P' && cpuname[i+3] == 'U') {
            cpuname[i] = 0;
            break;
        }
    }
    */
   if(!PRINT_CPU_FREQ) *(strstr(cpuname, " @")) = 0;
    printf("%s", cpuname);
    //fputs(CPU, stdout)
}

void gpu() {            // prints the current GPU
    printf("%-11s\e[0m%s", "GPU:", GPU);
}

void memory() {         // prints the used memory in the format used MiB / total MiB (XX%)
    struct sysinfo info;
    sysinfo(&info);

    unsigned long total = info.totalram;
    char used_str[15];

    // would be way more elegant, but leaves a slight gap? idk, I could also use /proc/meminfo
    //unsigned long used = total - info.freeram - info.bufferram - info.sharedram;

    //printf(COLOR "%s" SPACING "Memory:\e[0m %lu MiB / %lu MiB (%lu%%)\n", logo[14], used/1048576, total/1048576, (used * 100) / (1048576*total));

    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("sh", "sh", "-c", "free --byte | grep M | awk '{print $3}'", NULL);  // using free to count the used memory, 3rd arg
    } else {
        printf("%-11s\e[0m", "Memory:");
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], used_str, 14);
    used_str[read(pipes[0], used_str, 14) - 1] = 0;
    uint64_t used = atol(used_str);

    close(pipes[0]);
    
    printf("%ld MiB / %lu MiB (%ld%%)", used/1048576, total/1048576, (used * 100) / total);
}

void public_ip() {      // get the public IP adress
    char public_ip[20];

    int pipes[2];
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("curl", "curl", "-s", "ident.me", NULL);        // using curl --silent to get the Public IP aress
    } else {
        printf("%-11s\e[0m", "Public IP:");
    }
    wait(NULL);
    close(pipes[1]);

    
    public_ip[read(pipes[0], public_ip, 20)] = 0;

    close(pipes[0]);
    printf("%s", public_ip);
}

void local_ip() {      // get the local IP adress - WORK IN PROGRESS
    
}

int main(const int argc, char **argv) {
    static bool help = 0;
    static short line = 0;

    for(int i = 0; i < argc; i++) {
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help = 1;
        } else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--color")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1],"black")) {
                    color = "\e[30m";
                } else if(!strcmp(argv[i+1],"red")) {
                    color = "\e[31m";
                } else if(!strcmp(argv[i+1],"green")) {
                    color = "\e[32m";
                } else if(!strcmp(argv[i+1],"yellow")) {
                    color = "\e[33m";
                } else if(!strcmp(argv[i+1],"blue")) {
                    color = "\e[34m";
                } else if(!strcmp(argv[i+1],"purple")) {
                    color = "\e[35m";
                } else if(!strcmp(argv[i+1],"cyan")) {
                    color = "\e[36m";
                } else if(!strcmp(argv[i+1],"shell")) {
                    color = "\e[0m";
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid color! Use --help for more info\n", stderr);
                    user_is_an_idiot = true;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --color requires a color! Use --help for more info\n", stderr);
                return 1;
            }
        } else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bold")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "on")) {
                    bold = "\e[1m";
                } else if(!strcmp(argv[i+1], "off")) {
                    bold = "";
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid value for --bold! Use --help for more info\n", stderr);

                    return 2;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --bold requires a value! Use --help for more info\n", stderr);

                return 2;
            }
        } else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--logo")) {
            if(argv[i+1]) {
                if(!strcmp(argv[i+1], "arch")) {
                    logo = archlinux;
                } else if(!strcmp(argv[i+1], "debian")) {
                    logo = debian;
                } else {
                    fputs("\e[31m\e[1mERROR\e[0m: invalid value for --logo! Use --help for more info\n", stderr);
                    return 3;
                }
            } else {
                fputs("\e[31m\e[1mERROR\e[0m: --logo requires a value! Use --help for more info\n", stderr);
                return 3;
            }
        }
    }

    if(help) {  // print the help message if --help was used and exit
        printf("%s%salbafetch\e[0m - a system fetch utility\n", color, bold);
        printf("\n%s%sFLAGS\e[0m:\n", color, bold);
        printf("\t%s%s-h\e[0m,%s%s --help\e[0m:\t Print this help menu and exit\n", color, bold, color, bold);
        printf("\t%s%s-c\e[0m,%s%s --color\e[0m:\t Change the output color (default: cyan) [black, red, green, yellow, blue, purple, cyan, shell]\n", color, bold, color, bold);
        printf("\t%s%s-b\e[0m,%s%s --bold\e[0m:\t Specifies if bold should be used in colored parts (default: on) [on, off]\n", color, bold, color, bold);
        printf("\t%s%s-l\e[0m,%s%s --logo\e[0m:\t Changes the logo that will be displayed (default: arch) [arch, debian]\n", color, bold, color, bold);

        printf("\nReport a bug: %s%shttps://github.com/alba4k/albafetch/issues\e[0m\n", color, bold);

        return 0;
    }

    /* Adding modules:
    printf("%s%s%s" SPACING, color, bold, logo[line]);
    module();   // only change this part between lines
    line++;
    */

    printf("%s%s%s" SPACING, color, bold, logo[line]);
    title(color, bold);
    line++; 

    printf("%s\n%s%s" SPACING, color, bold, logo[line]);
    separator();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    uptime();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    separator();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    os();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    kernel();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    desktop();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    shell();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    term();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    packages();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    separator();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    host();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    cpu();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    gpu();
    line++; 

    printf("%s%s\n%s" SPACING, color, bold, logo[line]);
    memory();
    line++; 

    // ******** remaining lines of the logo ********
    while(logo[line]) {
        printf("%s%s\n%s\e[0m", color, bold, logo[line]);
        line++;
    }
    printf("\n");
}