#ifndef VARS_H
#define VARS_H

// CONFIGURATION OPTIONS:
extern char *separator_string;  // what is used as separator between sections
extern char *dash;              // color of the dash
extern char *dash_color;        // default separator

extern bool print_cpu_freq;

extern char *color;
extern char *bold;

// Labels:
extern char *host_name_label;
extern char *user_label;
extern char *uptime_label;
extern char *os_label;
extern char *kernel_label;
extern char *desktop_label;
extern char *shell_label;
extern char *term_label;
extern char *packages_label;
extern char *host_label;
extern char *bios_label;
extern char *cpu_label;
extern char *gpu_label;
extern char *mem_label;
extern char *pub_ip_label;
extern char *priv_ip_label;

typedef struct {
    char separator[32];
    char dash[32];
    char dash_color[32];
    bool print_cpu_freq;
} Config;

extern Config config;

#endif
