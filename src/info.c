#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <stdio.h>      
#include <ifaddrs.h>
#include <string.h> 
#include <arpa/inet.h>

#include "info.h"
#include "config.h"

void separator() {      // prints a separator
    fputs(SEPARATOR, stdout);
}

void title() {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX);
    
    static char username[LOGIN_NAME_MAX + 1] = "";
    getlogin_r(username, LOGIN_NAME_MAX);
    if(!username[0]) {
        int pipes[2];
        pipe(pipes);
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("whoami", "whoami", NULL);
        }
        wait(NULL);
        close(pipes[1]);

        username[read(pipes[0], username, LOGIN_NAME_MAX) - 1] = 0;
        close(pipes[0]);
    }

    printf("%s\e[0m\e[97m@%s%s%s\e[0m\e[97m", username, color, bold, hostname);
}

void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%-16s\e[0m\e[97m %s", HOSTNAME_LABEL DASH_COLOR DASH, hostname);
}

void user() {           // get the current login
    static char username[LOGIN_NAME_MAX + 1] = "";
    getlogin_r(username, LOGIN_NAME_MAX);
    if(!username[0]) {
        int pipes[2];
        pipe(pipes);
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("whoami", "whoami", NULL);
        }
        wait(NULL);
        close(pipes[1]);

        username[read(pipes[0], username, LOGIN_NAME_MAX) - 1] = 0;

        close(pipes[0]);
    }
    printf("%-16s\e[0m\e[97m %s", USER_LABEL DASH_COLOR DASH, username);
}

void uptime() {         // prints the uptime
    struct sysinfo info;
    sysinfo(&info);

    long secs = info.uptime;            // total uptime in seconds
    long days = secs/86400;
    char hours = secs/3600 - days*24;
    char mins = secs/60 - days*1440 - hours*60;
    char sec = secs - days*86400 - hours*3600 - mins*60;

    printf("%-16s\e[0m\e[97m", UPTIME_LABEL DASH_COLOR DASH);
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

    FILE *fp = fopen("/etc/os-release", "r");
    if(!fp) {
        fputs("[Missing /etc/os-release]", stderr);
        printf(" %s", name.machine);
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    char *str = malloc(len + 1);
    str[fread(str, 1, len, fp)] = 0;
    const char *field = "PRETTY_NAME=\"";
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

    printf("%-16s\e[0m\e[97m%s %s", OS_LABEL DASH_COLOR DASH, os_name, name.machine);

    fclose(fp);
    free(str);



    return;

    error:
        fputs("\e[0m\e[97m[Unrecognized file content]", stderr);
        printf(" %s", name.machine);
        fclose(fp);
        free(str);
        return;
}

void kernel() {         // prints the kernel version
    struct utsname name;
    uname(&name);

    printf("%-16s\e[0m\e[97m%s ", KERNEL_LABEL DASH_COLOR DASH, name.release);
}

void desktop() {        // prints the current desktop environment
        printf("%-16s\e[0m\e[97m%s", DESKTOP_LABEL DASH_COLOR DASH, getenv("XDG_CURRENT_DESKTOP")); // $XDG_CURRENT_DESKTOP
}

void shell() {          // prints the user default shell
    printf("%-16s\e[0m\e[97m%s", SHELL_LABEL DASH_COLOR DASH, getenv("SHELL"));        // $SHELL
}

void term() {           // prints the current terminal
    printf("%-16s\e[0m\e[97m%s", TERM_LABEL DASH_COLOR DASH, getenv("TERM"));     // $TERM
}

void packages() {       // prints the number of installed packages
    printf("%-16s\e[0m\e[97m", PACKAGES_LABEL DASH_COLOR DASH);

    char packages[10];
    char flatpaks[10];

    int pipes[2];
    int pipes2[2];

    pipe(pipes);
    pipe(pipes2);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes2[0]);
        dup2(pipes2[1], STDERR_FILENO);

        execlp("sh", "sh", "-c", "flatpak list | wc -l", NULL);        // using flatpak to list packages
    }
    wait(NULL);
    close(pipes[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    //size_t len = read(pipes[0], packages, 10);
    flatpaks[read(pipes[0], flatpaks, 10) - 1] = 0;

    pipe(pipes);
    pipe(pipes2);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes2[0]);
        dup2(pipes2[1], STDERR_FILENO);

        execlp("sh", "sh", "-c", "pacman -Qq | wc -l", NULL);        // using "pacman --query" to list the installed packages; using "wc --lines" to get the number of lines (wordcount)
    }
    wait(NULL);
    close(pipes[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 10) - 1] = 0;

    close(pipes[0]);
    if(packages[0] != '0') {
        printf("%s (pacman) ", packages);
        if(flatpaks[0] != '0')
            printf("%s (flatpak) ", flatpaks);
        return;
    }
    
    pipe(pipes);
    pipe(pipes2);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes2[0]);
        dup2(pipes2[1], STDERR_FILENO);

        execlp("sh", "sh", "-c", "apt list --installed | wc -l", NULL);        // using apt to list packages
    }
    wait(NULL);
    close(pipes[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 10) - 1] = 0;

    close(pipes[0]);
    if(packages[0] != '0') {
        printf("%s (apt) ", packages);
        if(flatpaks[0] != '0')
            printf("%s (flatpak) ", flatpaks);
        return;
    }

    pipe(pipes);
    pipe(pipes2);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes2[0]);
        dup2(pipes2[1], STDERR_FILENO);

        execlp("sh", "sh", "-c", "rpm -qa | wc -l", NULL);        // using rpm to list packages
    }
    wait(NULL);
    close(pipes[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    //size_t len = read(pipes[0], packages, 10);
    packages[read(pipes[0], packages, 10) - 1] = 0;

    close(pipes[0]);
    if(packages[0] != '0') {
        printf("%s (rpm) ", packages);
        if(flatpaks[0] != '0')
            printf("%s (flatpak) ", flatpaks);
        return;
    }

    if(flatpaks[0] != '0') {
        printf("%s (flatpak)", flatpaks);
        return;
    }
    
    fprintf(stderr, "[Unsupported]");
}

void host() {           // prints the current host machine
    FILE *fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if(!fp) {
        fputs("[Missing /sys/devices/virtual/dmi/id/product_name]", stderr);
        fclose(fp);
        return;
    }
    char *model = malloc(128);
    model[fread(model, 1, 0x10000, fp) - 1] = 0;

    fclose(fp);

    printf("%-16s\e[0m\e[97m%s", HOST_LABEL DASH_COLOR DASH, model);

    free(model);
}

void bios() {           // prints the current host machine
    printf("%-16s\e[0m\e[97m", BIOS_LABEL DASH_COLOR DASH);


    FILE *fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r");
    if(!fp) {
        fputs("[Missing /sys/devices/virtual/dmi/id/bios_vendor]", stderr);
        fclose(fp);
        return;
    }

    char *vendor = malloc(128);
    vendor[fread(vendor, 1, 0x10000, fp) - 1] = 0;

    printf("%s", vendor);
    free(vendor);

    fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r");
    if(!fp) {
        fputs("[Missing /sys/devices/virtual/dmi/id/bios_version]", stderr);
        fclose(fp);
        return;
    }

    char *version = malloc(128);
    version[fread(version, 1, 0x10000, fp) - 1] = 0;

    printf(" %s", version);
    free(version);

    fclose(fp);
}

void cpu() {            // prints the current CPU
    printf("%-16s\e[0m\e[97m", CPU_LABEL DASH_COLOR DASH);

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(!fp) {
        fputs("[Missing /proc/cpuinfo]", stderr);
        fclose(fp);
        return;
    }
    char *str = malloc(0x10000);
    str[fread(str, 1, 0x10000, fp)] = 0;
    fclose(fp);

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

    if(!PRINT_CPU_FREQ && cpu_info[0] == 'I') {
        end = strchr(cpu_info, '@');
        if(!end) {
            goto error;
        }
        end--;
    } else {
        end = strchr(cpu_info, '\n');
        if(!end) {
            goto error;
        }
    }
    

    (*end) = 0;

    printf("%s", cpu_info);
    //fputs(CPU, stdout)

    free(str);

    return;

    error:
        fputs("\e[0m\e[97m[Unrecognized file content]", stderr);
        fclose(fp);
        free(str);
        return;
}

void gpu() {            // prints the current GPU
    printf("%-16s\e[0m\e[97m%s", GPU_LABEL DASH_COLOR DASH, GPU);
}

void memory() {         // prints the used memory in the format used MiB / total MiB (XX%)
    struct sysinfo info;
    sysinfo(&info);

    printf("%-16s\e[0m\e[97m", MEM_LABEL DASH_COLOR DASH);

    unsigned long totalram = info.totalram / 1024;
    unsigned long freeram = info.freeram / 1024;
    unsigned long bufferram = info.bufferram / 1024;
    char used_str[15];
    char *str = malloc(0x1000);

    FILE *fp = fopen("/proc/meminfo", "r");     // open the file and copy its contents into str
    if(!fp) {
        fputs("[Missing /proc/meminfo]", stderr);
        fclose(fp);
        return;
    }
    str[fread(str, 1, 0x1000, fp)] = 0;
    fclose(fp);

    char *cachedram = strstr(str, "Cached");
    if(!cachedram) {
        goto error;
    }

    cachedram = strchr(cachedram, ':');
    if(!cachedram) {
        goto error;
    }
    cachedram += 2;

    char *end;
    end = strstr(cachedram, "kB");
    if(!end) {
        goto error;     
    }
    free(str);
    end--;
    (*end) = 0;

    unsigned long usedram = totalram - freeram - bufferram - atol(cachedram);

    printf("%lu MiB / %lu MiB (%lu%%)", usedram/1024, totalram/1024, (usedram * 100) / totalram);

    return;

    error:
        fputs("\e[0m\e[97m[Unrecognized file content]", stderr);
        fclose(fp);
        free(str);
        return;
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
        printf("%-16s\e[0m\e[97m", PUB_IP_LABEL DASH_COLOR DASH);
    }
    wait(NULL);
    close(pipes[1]);

    
    public_ip[read(pipes[0], public_ip, 20)] = 0;

    close(pipes[0]);
    printf("%s", public_ip);
}

void local_ip() {      // get the local IP adress
    struct ifaddrs *ifAddrStruct=NULL;
    struct ifaddrs *ifa=NULL;
    
    void *tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(strcmp(addressBuffer, "127.0.0.1")) 
                printf("%-16s\e[0m\e[97m%s", PRIV_IP_LABEL DASH_COLOR DASH, addressBuffer);
        }
    } 
}