#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>     // basename
#include <sys/wait.h>

#ifdef __APPLE__
#include "bsdwrap.h"
#include "macos_infos.h"
#else
#include <sys/sysinfo.h> // uptime, memory
#include <pci/pci.h>    // gpu
#endif // __APPLE__

#include <sys/utsname.h> // uname
#include <pwd.h>        // username
#include <ifaddrs.h>    // local ip
#include <arpa/inet.h>  // local ip
#include <time.h>       // date
#include <curl/curl.h>  // public ip
#include <dirent.h>     // packages

#include "info.h"
#include "queue.h"
#include "utils.h"

// just in case + vscode being annoying
#ifndef DT_DIR
#define _BSD_SOURCE
#define __USE_MISC
#ifndef DT_DIR
#define DT_DIR 4
#endif
#endif

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

    strncpy(dest, hostname, 256);

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
        if(sysinfo(&info))
            return 1;

        const long uptime = info.uptime;
    #endif // __APPLE__

    long days = uptime/86400;
    char hours = uptime/3600 - days*24;
    char mins = uptime/60 - days*1440 - hours*60;

    char result[256] = "";
    char str[24] = "";

    if(days) {
        snprintf(str, 24, "%ldd ", days);     // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(hours) {
        snprintf(str, 24, "%dh ", hours);      // print the number of days passed if more than 0
        strcat(result, str);
    }
    if(mins) {
        snprintf(str, 24, "%dm ", mins);       // print the number of minutes passed if more than 0
        strcat(result, str);
    }
    else if(uptime < 60) {
        snprintf(str, 24, "%lds", uptime);       // print the number of seconds passed if less than 60
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
    #ifdef __APPLE__
        strcpy("Aqua", dest);
    #else
        char *desktop = getenv("SWAYSOCK") ? "Sway" :
                             (desktop = getenv("XDG_CURRENT_DESKTOP")) ? desktop :
                             (desktop = getenv("DESKTOP_SESSION")) ? desktop :
                             getenv("KDE_SESSION_VERSION") ? "KDE" :
                             getenv("GNOME_DESKTOP_SESSION_ID") ? "GNOME" :
                             getenv("MATE_DESKTOP_SESSION_ID") ? "mate" :
                             getenv("TDE_FULL_SESSION") ? "Trinity" :
                             // !strcmp("linux", getenv("TERM")) ? "none" :      // what happens when running in tty
                             NULL;
        if(!desktop)
            return 1;
    #endif

    strcpy(dest, desktop);
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

            strncpy(dest, config.shell_path ? shell : basename(shell), 256);
            fclose(fp);
            return 0;
        }
    #endif

    char *shell = getenv("SHELL");
    if(shell && shell[0]) {
        strncpy(dest, config.shell_path ? shell : basename(shell), 256);
        return 0;
    }

    return 1;
}

// get the current login shell
int login_shell(char *dest) {
    char *shell = getenv("SHELL");

    if(shell && shell[0]) {
        strncpy(dest, config.shell_path ? shell : basename(shell), 256);
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

// TODO: packages (pacman - dpkg - flatpak - snap - rpm)
int packages(char *dest) {
    dest[0] = 0;
    char buf[256], str[64];
    DIR *dir;
    struct dirent *entry;
    unsigned count = 0;
    int pipes[2];
    bool done = false;

    #ifndef __APPLE__   // package managers that won't run on macOS
        FILE *fp;

        if(config.pkg_pacman && (dir = opendir("/var/lib/pacman/local"))) {
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    ++count;

            if(count) {
                snprintf(buf, 256 - strlen(buf), "%s%u%s", done ? ", " : "", count, config.pkg_mgr ? " (pacman)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
            closedir(dir);
        }

        if(config.pkg_dpkg && (fp = fopen("/var/lib/dpkg/status", "r"))) {   // alternatively, I could use "dpkg-query -f L -W" and strlen
            fseek(fp, 0, SEEK_END);
            size_t len = ftell(fp);
            rewind(fp);

            char *dpkg_list = malloc(len);
            dpkg_list[fread(dpkg_list, 1, len, fp) - 1] = 0;

            fclose(fp);

            count = 0;
            // this will be wrong if some package does not have "\nInstalled-Size: "
            // or if some package (for some reason) has it in the package description
            while(strstr(dpkg_list, "\nInstalled-Size: "))
                ++count;
            free(dpkg_list);

            if(count) {
                snprintf(dest, 256, "%u%s", count, config.pkg_mgr ? " (dpkg)" : "");
                done = true;
            }
        }

        if(config.pkg_rpm && !access("/bin/rpm", F_OK)) {
            pipe(pipes);

            if(!fork()) {
                close(*pipes);
                dup2(pipes[1], STDOUT_FILENO);

                execlp("sh", "sh", "-c", "rpm -qa 2>/dev/null | wc -l", NULL); 
            }
            wait(0);
            close(pipes[1]);
            str[read(pipes[0], str, 16) - 1] = 0;
            close(pipes[0]);

            if(str[0] != '0' && str[0]) {
                snprintf(buf, 256 - strlen(buf), "%s%s%s", done ? ", " : "", str, config.pkg_mgr ? " (rpm)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }

        if(config.pkg_flatpak && (dir = opendir("/var/lib/flatpak/runtime"))) {
            count = 0;
            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    ++count;

            if(count) {
                snprintf(buf, 256 - strlen(buf), "%s%u%s", done ? ", " : "", count, config.pkg_mgr ? " (flatpak)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
            closedir(dir);
        }

        if(config.pkg_snap && !access("/bin/snap", F_OK)) {
            pipe(pipes);

            if(!fork()) {
                close(*pipes);
                dup2(pipes[1], STDOUT_FILENO);

                execlp("sh", "sh", "-c", "snap list 2>/dev/null | wc -l", NULL); 
            }

            wait(0);
            close(pipes[1]);
            str[read(pipes[0], str, 16) - 1] = 0;
            close(pipes[0]);

            if(str[0] != '0' && str[0]) {
                snprintf(buf, 256 - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-1, config.pkg_mgr ? " (snap)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }
        }
    #endif

    if(!config.pkg_brew && (access("/usr/local/bin/brew", F_OK) || !access("/opt/homebrew/bin/brew", F_OK) || !access("/bin/brew", F_OK))) {
        pipe(pipes);

        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);
            execlp("brew", "brew", "--cellar", NULL); 
        }

        wait(0);
            
        close(pipes[1]);
        str[read(pipes[0], str, 64) - 1] = 0;
        close(pipes[0]);

        if((dir = opendir(str))) {
            count = 0;

            while((entry = readdir(dir)) != NULL)
                if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                    ++count;

            if(count) {
                snprintf(buf, 256, "%s%u%s", done ? ", " : "", count, config.pkg_mgr ? " (brew)" : "");
                done = true;
                strncat(dest, buf, 256 - strlen(dest));
            }

            closedir(dir);
        }
    }

    if(config.pkg_pip && !access("/bin/pip", F_OK)) {
        pipe(pipes);

        if(!fork()) {
            close(*pipes);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("sh", "sh", "-c", "pip list 2>/dev/null | wc -l", NULL); 
        }

        wait(0);
        close(pipes[1]);
        str[read(pipes[0], str, 16) - 1] = 0;
        close(pipes[0]);

        if(str[0] != '0' && str[0]) {
            snprintf(buf, 256 - strlen(buf), "%s%d%s", done ? ", " : "", atoi(str)-2, config.pkg_mgr ? " (pip)" : "");
            done = true;
            strncat(dest, buf, 256 - strlen(dest));
        }
    }

    if(done)
        return 0;
    return 1;
}

// gets the machine name and eventually model version
int host(char *dest) {
    #ifdef __APPLE__
        size_t BUF_SIZE = 256;
        sysctlbyname("hw.model", dest, &BUF_SIZE, NULL, 0);
        return 0;
    #else
        
    char *name = NULL, *version = NULL;
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

    if(name && version) {
        snprintf(dest, 256, "%s %s", name, version);
    } else if(name) {
        strncpy(dest, name, 256);
    } else if(version) {
        strncpy(dest, version, 256);
    } else
        return 1;

    free(name);
    free(version);
    
    
    return 0;
    #endif
}

// gets the current BIOS vendor and version (Linux only!)
int bios(char *dest) {
    #ifdef __APPLE__
    return 1;
    #else
    char *vendor = NULL, *version = NULL;
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

    if(vendor && version) {
        snprintf(dest, 256, "%s %s", vendor, version);
    } else if(vendor) {
        strncpy(dest, vendor, 256);
    } else if(version) {
        strncpy(dest, version, 256);
    } else
        return 1;

    free(vendor);
    free(version);
    
    return 0;
    #endif
}

// get the cpu name and frequency
int cpu(char *dest) {
    char buf[256] = "";
    char *cpu_info;
    char *end;

    #ifdef __APPLE__
    size_t BUF_SIZE = 256;
    sysctlbyname("machdep.cpu.brand_string", buf, &BUF_SIZE, NULL, 0);

    if(!buf[0])
        return 1;

    if(!config.cpu_freq) {
        if((end = strstr(buf, " @")))
            *end = 0;
    }
    #else
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(!fp) {
        return 1;
    }

    read_after_sequence(fp, "model name", buf, 256);
    fclose(fp);
    if(!(buf[0])) {
        return 1;
    }
    cpu_info = buf + 2;

    if((end = strstr(cpu_info, " @"))) {
        *end = 0;
    }

    if(!end) {
        end = strchr(cpu_info, '\n');
        if(!end) { 
            return 1;
        }
        *end = 0;
    }

    // Printing the clock frequency the first thread is currently running at
    char *cpu_freq, freq[24];
    end += 1;
    if((cpu_freq = strstr(end, "cpu MHz")) && config.cpu_freq) {

        cpu_freq = strchr(cpu_freq, ':');
        if(cpu_freq) {
            cpu_freq += 2;

            end = strchr(cpu_freq, '\n');
            if(end) {
                *end = 0;

                snprintf(freq, 24, " @ %g GHz", (float)(atoi(cpu_freq)/100) / 10);
                strcat(cpu_info, freq);
            }
        }
    }
    #endif

    // cleaning the string from various garbage
    if((end = strstr(cpu_info, "(R)")))
        memmove(end, end+3, strlen(end+3)+1);
    if((end = strstr(cpu_info, "(TM)")))
        memmove(end, end+4, strlen(end+4)+1);
    if((end = strstr(cpu_info, " CPU")))
        memmove(end, end+4, strlen(end+4)+1);
    if((end = strstr(cpu_info, "th Gen ")))
        memmove(end-2, end+7, strlen(end+7)+1);
    if((end = strstr(cpu_info, "-Core Processor"))) {
        end -= 4;
        end = strchr(end, ' ');
        *end = 0;
    }

    if(!config.cpu_brand) {
        if((end = strstr(cpu_info, "Intel Core ")))
            memmove(end, end+11, strlen(end+1));
        #ifdef __APPLE
        else if((end = strstr(cpu_info, "Apple ")))
            memmove(end, end+6, strlen(end+6)+1);
        #endif
        else if((end = strstr(cpu_info, "AMD ")))
            memmove(end, end+4, strlen(end+1));
    }

    strncpy(dest, cpu_info, 256);

    return 0;
}

// get the first gpu
int gpu(char *dest) {
    char *gpu_string = NULL;
    char *end;

    #ifdef __APPLE__
        struct utsname name;
        uname(&name);

        if(strcmp(name.machine, "arm64")) {
            gpu_string = get_gpu_string();
            if(!gpu_string)
                return 1;
        } else {
            char buf[1024];
            int pipes[2];
            pipe(pipes);

            if(!fork()) {
                dup2(pipes[1], STDOUT_FILENO);
                close(pipes[0]);
                close(pipes[1]);
                execlp("/usr/sbin/system_profiler", "system_profiler", "SPDisplaysDataType", NULL);
            }
            close(pipes[1]);
            wait(0);
            size_t bytes = read(pipes[0], buf, 1024);
            close(pipes[0]);

            if(bytes < 1) {
                return 1;
            }

            gpu_string = strstr(buf, "Chipset Model: ");
            if(!gpu_string)
                return 1;
            gpu_string += 15;
            char *end = strchr(gpu_string, '\n');
            if(!end)
                return 1;
            *end = 0;
        }
    #else
        // based on https://github.com/pciutils/pciutils/blob/master/example.c

        char device_class[256], namebuf[256];
        struct pci_access *pacc;
        struct pci_dev *dev;

        pacc = pci_alloc();		// get the pci_access structure
        pci_init(pacc);		// initialize the PCI library
        pci_scan_bus(pacc);		// we want to get the list of devices

        for (dev=pacc->devices; dev; dev=dev->next)	{ // iterates over all devices
            pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);	// fill in header info

            pci_lookup_name(pacc, device_class, 256, PCI_LOOKUP_CLASS, dev->device_class);
            if(!strcmp(device_class, "VGA compatible controller") || !strcmp(device_class, "3D controller")) {
                // look up the full name of the device
                gpu_string = pci_lookup_name(pacc, namebuf, 256, PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
                break;
            }
        }

        pci_cleanup(pacc);  // close everything
    #endif

    if((end = strstr(gpu_string, "Intel ")))
        gpu_string += 6;
    else if((end = strstr(gpu_string, "AMD ")))
        gpu_string += 4;
    else if((end = strstr(gpu_string, "Apple ")))
        gpu_string += 6;

    if((end = strstr(gpu_string, " Integrated Graphics Controller")))
        *end = 0;

    if(gpu_string) {
        strncpy(dest, gpu_string, 256);
        return 0;
    }

    return 1;
}

// get used and total memory
int memory(char *dest) {
    #ifdef __APPLE__ 
        bytes_t usedram = used_mem_size();
        bytes_t totalram = system_mem_size();

        if(!usedram || !totalram) { 
            return 1;
        }

        snprintf(dest, 200, "%llu MiB / %llu MiB", usedram/1048576, totalram/1048576);
    #else
        struct sysinfo info;
        if(sysinfo(&info))
            return 1;

        unsigned long totalram = info.totalram / 1024;
        unsigned long freeram = info.freeram / 1024;
        unsigned long sharedram = info.sharedram / 1024;

        FILE *fp = fopen("/proc/meminfo", "r");

        if(!fp) {
            return 1;
        }

        char buf[256];
        char *cachedram = buf; 

        read_after_sequence(fp, "Cached:", buf, 256);
        fclose(fp);

        if(!(buf[0])) {
            return 1;
        }
        cachedram += 2;

        char *end = strstr(cachedram, " kB");
        
        if(!end) {
            return 1; 
        }
        
        *end = 0;

        unsigned long usedram = totalram - freeram - sharedram - atol(cachedram);

        snprintf(dest, 200, "%lu MiB / %lu MiB", usedram/1024, totalram/1024);
    #endif

    if(config.mem_perc) {
        char perc[56];
        snprintf(perc, 56, " (%lu%%)", (usedram * 100) / totalram);
        strcat(dest, perc);
    }

    return 0;
}

// gets the current public ip
int public_ip(char *dest) {
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if(!curl_handle) {
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);

        return 1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, "ident.me");
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);

        return 1;
    }

    strncpy(dest, chunk.memory, 256);

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);

    return 0;
}

// gets all local ips
int local_ip(char *dest) {
    struct ifaddrs *addrs=NULL;
    bool done = false;
    int buf_size = 256;
    
    getifaddrs(&addrs);

    while(addrs) {
       if(addrs->ifa_addr && addrs->ifa_addr->sa_family == AF_INET && (strcmp(addrs->ifa_name, "lo") || config.show_localdomain)) {  // checking if the ip is valid
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
    if(!getcwd(dest, 256))
        return 1;
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
