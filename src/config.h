#ifndef CONFIG_H
#define CONFIG_H

#include "logos.h"

// CONFIGURATION OPTIONS:
#define SPACING "    "                          // defines the distance between the logo and the infos
#define SEPARATOR "\e[0m------------------"     // defines what is used as separator between sections
#define DASH ":"

// options
#define PRINT_CPU_FREQ false

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
#define CPU_LABEL "CPU"
#define GPU_LABEL "GPU"
#define MEM_LABEL "Memory"
#define PUB_IP_LABEL "Public IP"
#define PRIV_IP_LABEL

// was just too lazy to do it right, so it's hard coded. fuck you <3
#define HOST "HP ProBook 440 G6"        // what will be printed as host
#define GPU "Intel UHD Graphics 620"    // what will be printed as gpu

// would reccomend also changing the --help display output to show the correct default
static char *color = "\e[36m";                 // default color. Can be changed with --color
static char *bold = "\e[1m";                   // text modifiers. Bold is \e[1m

// add more logos if you want, I couldn't care less
static char **logo = archlinux;

#endif