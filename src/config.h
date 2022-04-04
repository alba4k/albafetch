#include "logos.h"

// CONFIGURATION OPTIONS:
#define SPACING "    "                          // defines the distance between the logo and the infos
#define SEPARATOR "\e[0m------------------"     // defines what is used as separator between sections

// would reccomend also changing the --help display output to show the correct default
char *color = "\e[36m";                         // default color. Can be changed with --color
char *bold = "\e[1m";                           // text modifiers. Bold is \e[1m

// add more logos if you want, I couldn't care less
char **logo = archlinux;

// actual config options
#define PRINT_CPU_FREQ false

// was just too lazy to do it right, so it's hard coded. fuck you <3
#define HOST "HP ProBook 440 G6"        // what will be printed as host
#define GPU "Intel UHD Graphics 620"    // what will be printed as gpu
//#define CPU "i5-8265U"                  // what will be printed as cpu. could get it from /proc/cpuinfo
