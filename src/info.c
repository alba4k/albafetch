#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>     // basename

#ifdef __APPLE__
#include "bsdwrap.h"
#include "macos_infos.h"
#else
#include <sys/sysinfo.h> // uptime, memory
#endif // __APPLE__

#include <sys/utsname.h> // uname
#include <pwd.h>        // username
#include <ifaddrs.h>    // local ip
#include <arpa/inet.h>  // local ip
#include <time.h>       // date
#include <curl/curl.h>  // public ip

#include "info.h"
#include "queue.h"
#include "utils.h"

// print the current user
int user(char *dest) {
    struct passwd *pw;

    unsigned uid = geteuid();
    if((int)uid == -1) {
        // couldn't get UID
        return 1;
    }

    pw = getpwuid(uid);

    strncpy(dest, pw->pw_name, LOGIN_NAME_MAX);

    return 0;
}

// print the machine hostname
int hostname(char *dest) {
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    strncpy(dest, hostname, HOST_NAME_MAX);

    return 0;
}

// print the current uptime
int uptime(char *dest) {
    #ifdef __APPLE__
        struct timeval boottime;
        int error;
        long uptime;
        error = sysctl_wrap(&boottime, sizeof(boottime), CTL_KERN, KERN_BOOTTIME);

        if(error < 0)
            return 1;

        time_t boot_seconds = boottime.tv_sec;
        time_t current_seconds = time(NULL);

        uptime = (long)difftime(current_seconds, boot_seconds);
    #else
        struct sysinfo info;
        sysinfo(&info);

        const long uptime = info.uptime;
    #endif // __APPLE__

    long days = uptime/86400;
    char hours = uptime/3600 - days*24;
    char mins = uptime/60 - days*1440 - hours*60;

    char result[24] = "";
    char str[16] = "";

    if(days) {
        snprintf(str, 16, "%ldd ", days);     // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(hours) {
        snprintf(str, 16, "%dh ", hours);      // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(mins) {
        snprintf(str, 16, "%dm ", mins);       // print the number of minutes passed if more than 0
        strcat(result, str);
    }
    else if(uptime < 60) {
        snprintf(str, 16, "%lds", uptime);       // print the number of seconds passed if less than 60
        strcat(result, str);
    }

    strncpy(dest, result, 256);

    return 0;
}

// print the operating system name and architechture (uname -m)
int os(char *dest) {
    struct utsname name;
    uname(&name);

    #ifdef __APPLE__
        snprintf(dest, 256, "macOS %s", name.machine);
        return 0;
    #else
        FILE *fp = fopen("/etc/os-release", "r");
        if(!fp) {
            return 1;
        }

        char buf[64];
        char *os_name = buf;
        char *end;

        read_after_sequence(fp, "PRETTY_NAME", buf, 64);
        fclose(fp);

        if(!buf[0])
            return 1;

        if(os_name[0] == '"' || os_name[0] == '\'')
            ++os_name;
        
        if(!(end = strchr(os_name, '\n')))
            return 1;
        *end = 0;

        if((end = strchr(os_name, '"')))
            *end = 0;
        else if((end = strchr(os_name, '\'')))
            *end = 0;

        snprintf(dest, 256, "%s %s", os_name, name.machine);

        return 0;
    #endif // __APPLE__
}

// print the running kernel version (uname -r)
int kernel(char *dest) {
    struct utsname name;
    uname(&name);

    snprintf(dest, 256, "%s", name.release);

    return 0;
}

// get the current desktop environnment
int desktop(char *dest) {
    char *desktop;

    #ifdef __APPLE__
        dekstop = "Aqua"
    #else
        desktop = getenv("SWAYSOCK") ? "Sway" :
                             (desktop = getenv("XDG_CURRENT_DESKTOP")) ? desktop :
                             (desktop = getenv("DESKTOP_SESSION")) ? desktop :
                             getenv("KDE_SESSION_VERSION") ? "KDE" :
                             getenv("GNOME_DESKTOP_SESSION_ID") ? "GNOME" :
                             getenv("MATE_DESKTOP_SESSION_ID") ? "mate" :
                             getenv("TDE_FULL_SESSION") ? "Trinity" :
                             // !strcmp("linux", getenv("TERM")) ? "none" :      // what happens when running in tty
                             NULL;
        if(!desktop) {
            return 1;
        }
    #endif

    snprintf(dest, 256, "%s", desktop);
    return 0;
}

// get the parent process (normally the shell)
int shell(char *dest) {
    #ifdef __linux__
        char path[32];

        sprintf(path, "/proc/%d/cmdline", getppid());

        FILE *fp = fopen(path, "r");
        if(fp) {
            char shell[256];
            shell[fread(shell, 1, 255, fp)] = 0;

            if(shell[0] == '-') // cmdline is "-bash" when login shell
                memcpy(shell, shell+1, strlen(shell+1)+1);

            snprintf(dest, 256, "%s", config.print_shell_path ? shell : basename(shell));
            fclose(fp);
            return 0;
        }
    #endif

    char *shell =  getenv("SHELL");
    if(shell && shell[0]) {
        snprintf(dest, 256, "%s", shell);
        return 0;
    }

    return 1;
}

// get the current login shell
int login_shell(char *dest) {
    char *shell = getenv("SHELL");

    if(shell && shell[0]) {
        snprintf(dest, 256, "%s", shell);
        return 0;
    }

    return 1;
}

// print the current terminal
int term(char *dest) {
    char *terminal = getenv("TERM");
    if(terminal) {
        terminal = strcmp("xterm-kitty", terminal) ? terminal : "kitty";

        snprintf(dest, 256, "%s", terminal);
        return 0;
    }

    return 1;
}

// TODO: packages

// gets the machine name and eventually model version
int host(char *dest) {
    #ifdef __APPLE__
        const size_t BUF_SIZE = 256;
        sysctlbyname("hw.model", dest, (size_t*)&BUF_SIZE, NULL, 0);
        return 0;
    #else
        
    char *name = NULL;
    char *version = NULL;
    FILE *fp = NULL;
    size_t len = 0;

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        name = malloc(len);
        name[fread(name, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        version = malloc(len);
        version[fread(version, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if(name && version)
        snprintf(dest, 256, "%s %s", name, version);
    else if(name)
        snprintf(dest, 256, "%s", name);
    else if(version)
        snprintf(dest, 256, "%s", version);
    else
        return 1;
    
    return 0;
    #endif
}

// gets the current BIOS vendor and version
int bios(char *dest) {
    #ifdef __APPLE__
    return 1;
    #else
    char *vendor = NULL;
    char *version = NULL;
    FILE *fp = NULL;
    size_t len = 0;

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        vendor = malloc(len);
        vendor[fread(vendor, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        version = malloc(len);
        version[fread(version, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if(vendor && version)
        snprintf(dest, 256, "%s %s", vendor, version);
    else if(vendor)
        snprintf(dest, 256, "%s", vendor);
    else if(version)
        snprintf(dest, 256, "%s", version);
    else
        return 1;
    
    return 0;
    #endif
}

// TODO: cpu

// TODO: gpu

// TODO: memory

// gets the current public ip
int public_ip(char *dest) {
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if(curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, "ident.me");
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if(res == CURLE_OK) {
            strncpy(dest, chunk.memory, 256);

            curl_easy_cleanup(curl_handle);
            free(chunk.memory);

            return 0;
        }

        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
    }
    return 1;
}

// gets all local ips
int local_ip(char *dest) {
    struct ifaddrs *addrs=NULL;
    bool done = false;
    int buf_size = 256;

    // will be moved to config
    bool show_localdomain = false;
    
    getifaddrs(&addrs);

    while(addrs) {
       if(addrs->ifa_addr && addrs->ifa_addr->sa_family == AF_INET && (strcmp(addrs->ifa_name, "lo") || show_localdomain)) {  // checking if the ip is valid
            struct sockaddr_in *pAddr = (struct sockaddr_in *)addrs->ifa_addr;
            
            snprintf(dest, buf_size, "%s%s (%s)", done ? ", " : "", inet_ntoa(pAddr->sin_addr), addrs->ifa_name);
            dest += strlen(dest);
            buf_size -= strlen(dest);
            done = true;
        }

        addrs = addrs->ifa_next;
    }

    freeifaddrs(addrs);
    if(done)
        return 0;
    return 1;
}

// gets the current working directory
int pwd(char *dest) {
    getcwd(dest, 256);
    return 0;
}

// gets the current date and time
int date(char *dest) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(dest, 256, "%02d/%02d/%d %02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return 0;
}

// show the terminal color configuration
int colors(char *dest) {
    for(int i = 0; i < 8; ++i)
        sprintf(dest+8*i, "\e[4%dm   ", i);

    strcpy(dest+64, "\e[0m");

    return 0;
}
// show the terminal color configuration (light version)
int light_colors(char *dest) {
    for(int i = 0; i < 8; ++i)
        sprintf(dest+9*i, "\e[10%dm   ", i);

    strcpy(dest+72, "\e[0m");

    return 0;
}
