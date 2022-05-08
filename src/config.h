#ifndef CONFIG_H
#define CONFIG_H

#include "vars.h"
#include <stdlib.h>

// CONFIGURATION OPTIONS:
#define SPACING "    "                              // defines the distance between the logo and the infos
#define SEPARATOR "\e[0m\e[37m------------------"   // defines what is used as separator between sections
#define DASH_COLOR "\e[37m"                         // \e[39m or DEFAULT_COLOR to make it white or like the text
#define DASH ":"                                    // default separator

#define PRINT_CPU_FREQ true

// Labels:
#define HOSTNAME_LABEL "Hostname"
#define USER_LABEL "User"
#define UPTIME_LABEL "Uptime"
#define OS_LABEL "OS"
#define KERNEL_LABEL "Kernel"
#define DESKTOP_LABEL "Desktop"
#define SHELL_LABEL "Shell"
#define TERM_LABEL "Terminal"
#define PACKAGES_LABEL "Packages"
#define HOST_LABEL "Host"
#define BIOS_LABEL "BIOS"
#define CPU_LABEL "CPU"
#define GPU_LABEL "GPU"
#define MEM_LABEL "Memory"
#define PUB_IP_LABEL "Publ. IP"
#define PRIV_IP_LABEL "Priv. IP"

// add more logos in src/logos.h if you want, I couldn't care less

// example:
// #define DEFAULT_LOGO "debian"    // set debian as default logo
// #define DEFAULT_COLOR "\e[32m"   // set green as default text color
// #define DEFAULT_BOLD "\e[1m"     // use bold 

#define DEFAULT_LOGO ""             // use "" for OS default
#define DEFAULT_COLOR ""            // use "" for logo default
#define DEFAULT_BOLD "\e[1m"        // use "" to turn off

#endif
