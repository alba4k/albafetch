#include <unistd.h>
#include <sys/wait.h>

#ifdef __APPLE__
#include "bsdwrap.h"
#include "macos_infos.h"
#else
#include <sys/sysinfo.h>
#endif

#include <sys/utsname.h>
#include <pwd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#ifdef ARCH_BASED
#include <alpm.h>
#include <alpm_list.h>
#endif
#include <stdio.h>      
#include <string.h> 

#include "info.h"

// separator
void separator() {      // prints a separator
    printf("%s", config.separator);
}

// title
void title() {          // prints a title in the format user@hostname
    static char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX);
    
    struct passwd *pw;
    uid_t uid = geteuid();

    pw = uid == -1 && 0 ? NULL : getpwuid(uid);
    if(!pw) {
        fflush(stdout);
        fprintf(stderr, "%s%s[Unsupported]", config.color, config.bold);
        fflush(stderr);
        printf("\e[0m@%s%s%s\e[0m", config.color, config.bold, hostname);
        return;
    }
    char *username = pw->pw_name;

    printf("%s%s\e[0m@%s%s%s\e[0m", config.title_prefix, username, config.color, config.bold, hostname);
}

// hostname
void hostname() {       // getting the computer hostname (defined in /etc/hostname and /etc/hosts)
    char format[100];
    snprintf(format, 100, "%s%s%s", config.hostname_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    printf("%s", hostname);
}

// user
void user() {           // get the current login
    char format[100];
    snprintf(format, 100, "%s%s%s", config.user_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct passwd *pw;
    uid_t uid = geteuid();

    pw = uid == -1 ? NULL : getpwuid(uid);
    if(!pw) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
    }
    char *username = pw->pw_name;
    fputs(username, stdout);
}

// uptime
void uptime() {         // prints the uptime
    char format[100];
    long uptime;

    snprintf(format, 100, "%s%s%s", config.uptime_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);
    
    #ifdef __APPLE__
        struct timeval boottime;
        int error;
        error = sysctl_wrap(&boottime, sizeof(boottime), CTL_KERN, KERN_BOOTTIME);

        if(error < 0)
            uptime = 0;

        time_t boot_seconds = boottime.tv_sec;
        time_t current_seconds = time(NULL);

        uptime = (long)difftime(current_seconds, boot_seconds);
    #else
        struct sysinfo info;
        sysinfo(&info);

        uptime = info.uptime;
    #endif

    long days = uptime/86400;
    char hours = uptime/3600 - days*24;
    char mins = uptime/60 - days*1440 - hours*60;

    if(days) {
        printf("%ldd ", days);      // print the number of days passed if more than 0
    }
    if(hours) {
        printf("%dh ", hours);      // print the number of days passed if more than 0
    }
    if(mins) {
        printf("%dm ", mins);       // print the number of minutes passed if more than 0
    }
    else if(uptime < 60) {
        printf("%ds", uptime);       // print the number of seconds passed if less than 60
    }
}

//os
#ifdef __APPLE__
void os() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.os_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct utsname name;
    uname(&name);

    printf("macOS %s", name.machine);
}
#else
void os() {             // prints the os name + arch
    char format[100];
    snprintf(format, 100, "%s%s%s", config.os_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct utsname name;
    uname(&name);

    FILE *fp = fopen("/etc/os-release", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        printf(" %s", name.machine);
        return;
    }

    char buf[64];
    char *os_name = buf, *end;

    read_after_sequence(fp, "PRETTY_NAME", buf, 64);
    fclose(fp);
    if(os_name[0] == '"' || os_name[0] == '\'')
        ++os_name;
    
    end = strchr(os_name, '\n');
    if(!end) 
        goto error;
    *end = 0;

    if((end = strchr(os_name, '"'))) *end = 0;
    else if(!(end = strchr(os_name, '\''))) *end = 0;


    printf("%s %s", os_name, name.machine);

    return;

    error:
        fflush(stdout);
        fputs("[Bad Format]", stderr);
        fflush(stderr);
        printf(" %s", name.machine);
        return;
}
#endif

void kernel() {         // prints the kernel version
    char format[100];
    snprintf(format, 100, "%s%s%s", config.kernel_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct utsname name;
    uname(&name);

    printf("%s ", name.release);
}

//desktop
#ifdef __APPLE__
void desktop() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.desktop_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    printf("Aqua");
}
#else
void desktop() {        // prints the current desktop environment
    char format[100];
    snprintf(format, 100, "%s%s%s", config.desktop_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    const char *de;
    const char *desktop = getenv("SWAYSOCK") ? "Sway" :
                          (de = getenv("XDG_CURRENT_DESKTOP")) ? de :
                          (de = getenv("DESKTOP_SESSION")) ? de :
                          getenv("KDE_SESSION_VERSION") ? "KDE" :
                          getenv("GNOME_DESKTOP_SESSION_ID") ? "GNOME" :
                          getenv("MATE_DESKTOP_SESSION_ID") ? "mate" :
                          getenv("TDE_FULL_SESSION") ? "Trinity" :
                          !strcmp("linux", getenv("TERM")) ? "none" :
                          "[Unsupported]";

    printf("%s", desktop); 
}

#endif

// shell
void shell() {          // prints the user default shell
    char format[100];
    snprintf(format, 100, "%s%s%s", config.shell_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    char *shell = getenv("SHELL");
    char *ptr = strstr(shell, "/bin/");
    if(ptr) {
        shell = ptr + 5;
    }

    printf("%s", shell);
}
// terminal
void term() {           // prints the current terminal
    char format[100];
    snprintf(format, 100, "%s%s%s", config.term_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    char *terminal = getenv("TERM");
    terminal = strcmp("xterm-kitty", terminal) ? terminal : "kitty";
    

    printf("%s", terminal);
}

// packages
#ifndef __APPLE__
void packages() {       // prints the number of installed packages
    char format[100];
    snprintf(format, 100, "%s%s%s", config.packages_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    int pipes[2];
    char packages[10];
    bool supported;

    #ifdef ARCH_BASED
        alpm_errno_t err;
        alpm_handle_t *handle = alpm_initialize("/", "/var/lib/pacman/", &err);
        alpm_db_t *db_local = alpm_get_localdb(handle);
        size_t pkgs = 0;

        for(alpm_list_t *entry = alpm_db_get_pkgcache(db_local); entry; entry = alpm_list_next(entry))
            ++pkgs;

        alpm_release(handle);

        if(pkgs)
            printf("%ld (pacman) ", pkgs);
        supported = true;
    #endif
    if(!access("/usr/bin/dpkg-query", F_OK)) {
        pipe(pipes);

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "dpkg-query -f '.\n' -W | wc -l", NULL); 
        }
        wait(NULL);
        close(pipes[1]);
        packages[read(pipes[0], packages, 10) - 1] = 0;
        close(pipes[0]);

        if(packages[0] != '0')
            printf("%s (dpkg) ", packages);
        supported = true;
    }

    if(!access("/usr/bin/rpm", F_OK)) {
        pipe(pipes);

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "rpm -qa | wc -l", NULL); 
        }
        wait(NULL);
        close(pipes[1]);
        packages[read(pipes[0], packages, 10) - 1] = 0;
        close(pipes[0]);

        if(packages[0] != '0')
            printf("%s (rpm) ", packages);
        supported = true;
    }

    if(!access("/usr/bin/flatpak", F_OK)) {
        pipe(pipes);

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "flatpak list | wc -l", NULL); 
        }
        wait(NULL);
        close(pipes[1]);
        packages[read(pipes[0], packages, 10) - 1] = 0;
        close(pipes[0]);

        if(packages[0] != '0')
            printf("%s (flatpak) ", packages);
        supported = true;
    }

    if(!access("/usr/bin/snap", F_OK)) {
        pipe(pipes);

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "snap list 2> /dev/null | wc -l", NULL);   // sending stderr to null to avoid snap printing shit when installed with 0 pacakges
        }
        wait(NULL);
        close(pipes[1]);
        packages[read(pipes[0], packages, 10) - 1] = 0;
        close(pipes[0]);

        if(packages[0] != '0')
            printf("%d (snap) ", atoi(packages) - 1);
        supported = true;
    }

    if(!supported) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
    }
}
#else
void packages() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.packages_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    fflush(stdout);
    fputs("[Unsupported]", stderr);
    fflush(stderr);
}
#endif

// host
#ifdef __APPLE__
void host() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.host_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    printf("Apple");
}
#else
void host() {           // prints the current host machine
    char format[100];
    snprintf(format, 100, "%s%s%s", config.host_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    FILE *fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char model[len];
    model[fread(model, 1, len, fp) - 1] = 0;
    
    fclose(fp);

    printf("%s", model);
}
#endif

// bios
#ifdef __APPLE__
void packages() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.bios_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    fflush(stdout);
    fputs("[Unsupported]", stderr);
    fflush(stderr);
}
#else
void bios() {           // prints the current host machine
    char format[100];
    snprintf(format, 100, "%s%s%s", config.bios_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    bool errors = false;

    FILE *fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r");
    if(!fp) {
        errors = true;
    } else {
        fseek(fp, 0, SEEK_END);
        size_t len = ftell(fp);
        rewind(fp);

        char vendor[len];
        vendor[fread(vendor, 1, len, fp) - 1] = 0;

        printf("%s", vendor);
        fclose(fp);
    }

    fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r");
    if(!fp) {
        if(errors) {
            fflush(stdout);
            fputs("[Unsupported]", stderr);
            fflush(stderr);
            return;
        } else {
            return;
        }
    }

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char version[len];
    version[fread(version, 1, len, fp) - 1] = 0;

    printf(" %s", version);

    fclose(fp);

    return;        
}
#endif

// cpu
#ifdef __APPLE__
void cpu() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.cpu_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    size_t buf_size = 100;
    char buf[buf_size];
    sysctlbyname("machdep.cpu.brand_string", &buf, &buf_size, NULL, 0);

    printf("%s", buf);
}
#else
void cpu() {            // prints the current CPU
    char format[100];
    snprintf(format, 100, "%s%s%s", config.cpu_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(!fp) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }
    
    char buf[256];
    char *cpu_info = &buf[0];

    read_after_sequence(fp, "model name", buf, 256);
    fclose(fp);
    if(!buf) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }
    cpu_info += 2;

    char *end = strchr(cpu_info, '@');

    if(!end) {
        end = strchr(cpu_info, '\n');
        if(!end) {
            goto error;
        }
    } else {
        --end;
    }

    (*end) = 0;

    // cleaning the string from various garbage
    if(end = strstr(cpu_info, "(R)"))
        memmove(end, end+3, strlen(end+1));
    if(end = strstr(cpu_info, "(TM)"))
        memmove(end, end+4, strlen(end+1));
    if(end = strstr(cpu_info, "CPU"))
        memmove(end, end+3, strlen(end+1));
    if(end = strstr(cpu_info, "-Core Processor")) {
        end -= 4;
        end = strchr(end, ' ');
        *end = 0;
    }

    if(!config.print_cpu_brand) {
        if(end = strstr(cpu_info, "Intel Core"))
            memmove(end, end+11, strlen(end+1));
        if(end = strstr(cpu_info, "AMD Ryzen"))
            memmove(end, end+10, strlen(end+1));
    }

    printf("%s", cpu_info);

    if(config.print_cpu_freq) {
        *end = ' ';

        char *cpu_freq = strstr(cpu_info, "cpu MHz");
        if(!cpu_freq)
            goto error;

        cpu_freq = strchr(cpu_freq, ':');
        if(!cpu_freq)
            goto error;
        cpu_freq += 2;

        end = strchr(cpu_freq, '\n');
        if(!end)
            goto error;

        *end = 0;

        printf(" @ %g GHz", (float)(atoi(cpu_freq)/100)/10);
    }

    return;

    error:
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
}
#endif

// gpu
#ifdef __APPLE__
void gpu() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.gpu_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    const char *gpu_string = get_gpu_string();
    if(!gpu_string) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }
    printf("%s", gpu_string);
}
#else
void gpu() {            // prints the current GPU
    char format[100];
    snprintf(format, 100, "%s%s%s", config.gpu_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    if(!access("/usr/bin/lspci", F_OK)) {
        int pipes[2];
        char *lspci = malloc(0x2000);

        pipe(pipes);
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);
            execlp("lspci", "lspci", "-mm", NULL); 
        }
        wait(NULL);
        close(pipes[1]);
        lspci[read(pipes[0], lspci, 0x2000)] = 0;
        close(pipes[0]);

        char *gpu = strstr(lspci, "3D");
        if(!gpu) {
            gpu = strstr(lspci, "VGA");
            if(!gpu)
                goto error;
        }                           

        for(int i = 0; i < 4; ++i) {
            gpu = strchr(gpu, '"');
            if(!gpu)
                goto error;
            ++gpu;
            // VGA compatible controller" "Intel Corporation" "WhiskeyLake-U GT2 [UHD Graphics 620]"
            //  "Intel Corporation" "WhiskeyLake-U GT2 [UHD Graphics 620]"
            // Intel Corporation" "WhiskeyLake-U GT2 [UHD Graphics 620]"
            //  "WhiskeyLake-U GT2 [UHD Graphics 620]"
            // WhiskeyLake-U GT2 [UHD Graphics 620]"
        }

        char *end = strchr(gpu, '"');   // WhiskeyLake-U GT2 [UHD Graphics 620]
        if(!end)
            goto error;
        *end = 0;

        char *ptr = strchr(gpu, '[');
        if(ptr && !config.print_gpu_arch) {
            end = strchr(ptr, ']');
            if(!end) {
                printf("%s", gpu);
                free(lspci);
                return;
            }
            *end = 0;

            gpu = ptr + 1;
        }

        printf("%s", gpu);
        free(lspci);
        return;

        error:
            free(lspci);
            fflush(stdout);
            fputs("[Unsupported]", stderr);
            fflush(stderr);
            return;
    }
    fflush(stdout);
    fputs("[Unsupported]", stderr);
    fflush(stderr);
    return;
}
#endif

// memory
#ifdef __APPLE__ 
void memory() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.mem_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    bytes_t usedram = used_mem_size();
    bytes_t totalram = system_mem_size();

    if(usedram == 0 || totalram == 0) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }

    printf("%llu MiB / %llu MiB (%llu%%)", usedram/1048576, totalram/1048576, (usedram * 100) / totalram);
    return;
}
#else
void memory() {
    char format[100];
    snprintf(format, 100, "%s%s%s", config.mem_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct sysinfo info;
    sysinfo(&info);

    unsigned long totalram = info.totalram / 1024;
    unsigned long freeram = info.freeram / 1024;
    unsigned long bufferram = info.bufferram / 1024;

    FILE *fp = fopen("/proc/meminfo", "r");

    if(!fp) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }

    char buf[256];
    char *cachedram = buf; 

    read_after_sequence(fp, "Cached:", buf, 256);
    fclose(fp);

    if(!buf) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;
    }
    cachedram += 2;

    char *end;
    end = strstr(cachedram, " kB");
    
    if(!end) {
        fflush(stdout);
        fputs("[Unsupported]", stderr);
        fflush(stderr);
        return;     
    }
    
    (*end) = 0;

    unsigned long usedram = totalram - freeram - bufferram - atol(cachedram);

    printf("%lu MiB / %lu MiB (%lu%%)", usedram/1024, totalram/1024, (usedram * 100) / totalram);

    return;
}
#endif

// public IP adress
void public_ip() {      // get the public IP address
    char format[100];
    snprintf(format, 100, "%s%s%s", config.pub_ip_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

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

// local IP adress
void local_ip() {      // get the local IP address
    char format[100];
    snprintf(format, 100, "%s%s%s", config.loc_ip_label, config.dash_color, config.dash);
    if(config.align_infos) printf("%-16s\e[0m", format);
    else printf("%s\e[0m ", format);

    struct ifaddrs *ifAddrStruct=NULL;
    struct ifaddrs *ifa=NULL;
    
    void *tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if(!ifa->ifa_addr) {
            continue;
        }
        if(ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(strcmp(addressBuffer, "127.0.0.1")) 
                printf("%s", addressBuffer);
        }
    } 
}
