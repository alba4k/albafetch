#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef __linux__
#include <sys/sysinfo.h>
#else
#include <sys/sysctl.h>
#endif
#include <sys/utsname.h>
#include <pwd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include <stdio.h>      
#include <string.h> 

#include "info.h"
#include "config.h"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

// Not sure if this 
#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX HOST_NAME_MAX
#endif

void separator() {      // prints a separator
    fputs(SEPARATOR, stdout);
}

void title() {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX);
    
    struct passwd *pw;
    uid_t uid = geteuid();

    pw = uid == -1 && 0 ? NULL : getpwuid(uid);
    if(!pw) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
        return;
    }
    char *username = pw->pw_name;

    printf("%s\e[0m\e[97m@%s%s%s\e[0m\e[97m", username, color, bold, hostname);
}

void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%-16s\e[0m\e[97m %s", HOSTNAME_LABEL DASH_COLOR DASH, hostname);
}

void user() {           // get the current login
    // imagine stealing code from whoami, couldn't be me

    printf("%-16s\e[0m\e[97m", USER_LABEL DASH_COLOR DASH);

    struct passwd *pw;
    uid_t uid = geteuid();

    pw = uid == -1 && 0 ? NULL : getpwuid(uid);
    if(!pw) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
    }
    char *username = pw->pw_name;
    fputs(username, stdout);
}

#ifdef __APPLE__
static long macos_uptime() {
    struct timeval boottime;
    size_t len = sizeof(boottime);
    int mib[] = { CTL_KERN, KERN_BOOTTIME };

    int error = sysctl(mib, 2, &boottime, &len, NULL, 0);

    if (error < 0)
        return -1;

    time_t boot_seconds = boottime.tv_sec;
    time_t current_seconds = time(NULL);

    return (long) difftime(current_seconds, boot_seconds);

}
#endif

#ifdef __linux__
static long linux_uptime() {
    struct sysinfo info;
    sysinfo(&info);

    return info.uptime;
}
#endif

void uptime() {         // prints the uptime
    long uptime;  

#ifdef __linux    
    uptime = linux_uptime();
#else
    uptime = macos_uptime();
#endif

    long secs = uptime;            // total uptime in seconds
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

    printf("%-16s\e[0m\e[97m", OS_LABEL DASH_COLOR DASH);

    FILE *fp = fopen("/etc/os-release", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
        printf(" %s", name.machine);
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    char *str = malloc(len + 1);
    str[fread(str, 1, len, fp)] = 0;
    char *os_name = strstr(str, "PRETTY_NAME=\"");
    if(!os_name) {
        os_name = strstr(str, "PRETTY_NAME=\'");
        if(!os_name)
            goto error;
    }
    os_name += strlen("PRETTY_NAME=\"");
    *strstr(os_name, "ID") = 0;
    char *end = strchr(os_name, '"');
    if(!end) {
        end = strchr(os_name, '\'');
        if(!end)
            goto error;
    }
    *end = 0;

    printf("%s %s", os_name, name.machine);

    fclose(fp);
    free(str);



    return;

    error:
        fflush(stdout);
        fputs("[Bad Format]", stderr);
        fflush(stderr);
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
    char snaps[10];

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

        execlp("sh", "sh", "-c", "snap list | wc -l", NULL);        // using flatpak to list packages
    }
    wait(NULL);
    close(pipes[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    //size_t len = read(pipes[0], packages, 10);
    snaps[read(pipes[0], snaps, 10) - 1] = 0;

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
        printf("%s (pacman)", packages);
        if(flatpaks[0] != '0')
            printf(", %s (flatpak)", flatpaks);
        if(snaps[0] != '0')
            printf(", %d (snap)", atoi(snaps) - 1);
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
        printf("%s (apt)", packages);
        if(flatpaks[0] != '0')
            printf(", %s (flatpak)", flatpaks);
        if(snaps[0] != '0')
            printf(", %d (snap)", atoi(snaps) - 1);
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
        printf(", %s (rpm)", packages);
        if(flatpaks[0] != '0')
            printf(", %s (flatpak)", flatpaks);
        if(snaps[0] != '0')
            printf(", %d (snap)", atoi(snaps) - 1);
        return;
    }

    if(flatpaks[0] != '0') {
        printf("%s (flatpak)", flatpaks);
        if(snaps[0] != '0')
            printf(", %d (snap)", atoi(snaps) - 1);
        return;
    }
    
    if(snaps[0] != '0')
        printf("%s (snap)", snaps);
        return;

    fflush(stdout);
    fputs("[Unsupported]", stderr);
    fflush(stderr);
}

void host() {           // prints the current host machine
    printf("%-16s\e[0m\e[97m", HOST_LABEL DASH_COLOR DASH);

    FILE *fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
        return;
    }

    char model[128];
    model[fread(model, 1, 128, fp) - 1] = 0;

    fclose(fp);

    printf("%s", model);

}

void bios() {           // prints the current host machine
    printf("%-16s\e[0m\e[97m", BIOS_LABEL DASH_COLOR DASH);


    FILE *fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
        return;
    }

    char vendor[128];
    vendor[fread(vendor, 1, 128, fp) - 1] = 0;

    fclose(fp);

    printf("%s", vendor);

    fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
        return;
    }

    char version[128];
    version[fread(version, 1, 128, fp) - 1] = 0;

    printf(" %s", version);

    fclose(fp);
}

void cpu() {            // prints the current CPU
    printf("%-16s\e[0m\e[97m", CPU_LABEL DASH_COLOR DASH);

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(!fp) {
        fputs("[Not Found]", stderr);
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
        fflush(stdout);
        fputs("[Bad Format]", stderr);
        fflush(stderr);
        fclose(fp);
        free(str);
        return;
}

void gpu() {            // prints the current GPU
    printf("%-16s\e[0m\e[97m", GPU_LABEL DASH_COLOR DASH);
    printf("%s", GPU);
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
        fflush(stdout);
        fputs("[Not Found]", stderr);
        fflush(stderr);
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
        fflush(stdout);
        fputs("[Bad Format]", stderr);
        fflush(stderr);
        fclose(fp);
        free(str);
        return;
}

void public_ip() {      // get the public IP adress
    printf("%-16s\e[0m\e[97m", PUB_IP_LABEL DASH_COLOR DASH);

    char public_ip[20];
    int pipes[2];

    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        execlp("curl", "curl", "-s", "ident.me", NULL);        // using curl --silent to get the Public IP aress
    }
    wait(NULL);
    close(pipes[1]);

    
    public_ip[read(pipes[0], public_ip, 20)] = 0;

    close(pipes[0]);
    printf("%s", public_ip);
}

void local_ip() {      // get the local IP adress
    printf("%-16s\e[0m\e[97m", PRIV_IP_LABEL DASH_COLOR DASH);

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
                printf("%s", addressBuffer);
        }
    } 
}
