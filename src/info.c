#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h> // basename

#ifdef __APPLE__
#include "bsdwrap.h"
#include "macos_infos.h"
#else
#include <sys/sysinfo.h> // uptime, memory
#endif // __APPLE__

#include <sys/utsname.h> // uname
#include <pwd.h> // username

#include "info.h"
#include "queue.h"

// prints the current user
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

// prints the machine hostname
int hostname(char *dest) {
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    strncpy(dest, hostname, HOST_NAME_MAX);

    return 0;
}

// prints the current uptime
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

// prints the operating system name and architechture (uname -m)
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

// prints the running kernel version (uname -r)
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
