#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include "info.h"
#include "config.h"

char *color = DEFAULT_COLOR;
char *bold = DEFAULT_BOLD;

void separator() {      // prints a separator
    fputs(SEPARATOR, stdout);
}

void title() {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s\e[0m@%s%s%s\e[0m", getlogin(), color, bold, hostname);
}

void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%-15s\e[0m %s", HOSTNAME_LABEL DASH_COLOR DASH, hostname);
}

void user() {           // get the current login
    printf("%-15s\e[0m %s", USER_LABEL DASH_COLOR DASH, getlogin());
}

void uptime() {         // prints the uptime
    struct sysinfo info;
    sysinfo(&info);

    long secs = info.uptime;            // total uptime in seconds
    long days = secs/86400;
    char hours = secs/3600 - days*24;
    char mins = secs/60 - days*1440 - hours*60;
    char sec = secs - days*86400 - hours*3600 - mins*60;

    printf("%-15s\e[0m", UPTIME_LABEL DASH_COLOR DASH);
    if(days) {
        printf("%ldd ", days);     // print the number of days passed if more than 0
    }
    if(hours) {
        printf("%dh ", hours);       // print the number of days passed if more than 0
    }
    if(mins) {
        printf("%dm ", mins);        // print the number of minutes passed if more than 0
    }
    else if(secs < 60) {
        printf("%ds", sec);         // print the number of seconds passed if more than 0
    }
}

void os() {             // prints the os name + arch
    struct utsname name;
    uname(&name);

    FILE *f = fopen("/etc/lsb-release", "r");
    if(!f) {
        fputs("[Missing /etc/lsb-release]", stderr);
        printf(" %s", name.machine);
        return;
    }
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);
    char *str = malloc(len + 1);
    str[fread(str, 1, 0x10000, f)] = 0;
    const char *field = "DISTRIB_DESCRIPTION=\"";
    char *os_name = strstr(str, field);
    if(!os_name) {
        goto error;
    }
    os_name += strlen(field);
    char *end = strchr(os_name, '"');
    if(!end) {
        goto error;
    }
    *end = 0;

    printf("%-15s\e[0m%s %s", OS_LABEL DASH_COLOR DASH, os_name, name.machine);

    free(str);

    return;

    error:
        fputs("\e[0m[Unrecognized file content]\n", stderr);
        printf(" %s", name.machine);
        return;
}

void kernel() {         // prints the kernel version
    struct utsname name;
    uname(&name);

    printf("%-15s\e[0m%s ", KERNEL_LABEL DASH_COLOR DASH, name.release);
}

void desktop() {        // prints the current desktop environment
        printf("%-15s\e[0m%s", DESKTOP_LABEL DASH_COLOR DASH, getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP
}

void shell() {          // prints the user default shell
    printf("%-15s\e[0m%s", SHELL_LABEL DASH_COLOR DASH, getenv("SHELL"));        // $SHELL
}

void term() {           // prints the current terminal
    printf("%-15s\e[0m%s", TERM_LABEL DASH_COLOR DASH, getenv("TERM"));     // $TERM
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
        printf("%-15s\e[0m", PACKAGES_LABEL DASH_COLOR DASH);
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 10) - 1] = 0;

    close(pipes[0]);
    printf("%s (pacman)", packages);
}

void host() {           // prints the current host machine
    printf("%-15s\e[0m%s", HOST_LABEL DASH_COLOR DASH, HOST);
}

void cpu() {            // prints the current CPU
    printf("%-15s\e[0m", CPU_LABEL DASH_COLOR DASH);

    FILE *f = fopen("/proc/cpuinfo", "r");
    if(!f) {
        printf("[Where /proc/cpuinfo?]");
        return;
    }
    char *str = malloc(0x10000);
    str[fread(str, 1, 0x10000, f)] = 0;
    char *cpu_info = strstr(str, "model name");
    if(!cpu_info) {
        goto error;
    }

    cpu_info = strchr(cpu_info, ':');
    if(!cpu_info) {
        goto error;
    }
    cpu_info += 2;

    char *end;
    if(PRINT_CPU_FREQ) {
        end = strchr(cpu_info, '\n');
        if(!end) {
            goto error;
        }
    } else {
        end = strchr(cpu_info, '@');
        if(!end) {
            goto error;
        }
        end--;
    }
    

    (*end) = 0;

    printf("%s", cpu_info);
    //fputs(CPU, stdout)

    free(str);

    return;

    error:
        fputs("\e[0m[Unrecognized file content]\n", stderr);
        return;
}

void gpu() {            // prints the current GPU
    printf("%-15s\e[0m%s", GPU_LABEL DASH_COLOR DASH, GPU);
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
        printf("%-15s\e[0m", MEM_LABEL DASH_COLOR DASH);
    }
    wait(NULL);
    close(pipes[1]);

    //size_t len = read(pipes[0], used_str, 14);
    used_str[read(pipes[0], used_str, 14) - 1] = 0;
    long used = atol(used_str);

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
        printf("%-15s\e[0m", PUB_IP_LABEL DASH_COLOR DASH);
    }
    wait(NULL);
    close(pipes[1]);

    
    public_ip[read(pipes[0], public_ip, 20)] = 0;

    close(pipes[0]);
    printf("%s", public_ip);
}

void local_ip() {      // get the local IP adress - WORK IN PROGRESS
   printf("%-15s\e[0m", PRIV_IP_LABEL DASH_COLOR DASH); 
}