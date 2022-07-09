#ifndef VARS_H
#define VARS_H

typedef struct {
    char separator[65];
    char separator2[65];
    char dash[33];
    bool print_cpu_freq;
    bool print_cpu_brand;
    bool print_gpu_arch;
    bool print_mem_perc;
    bool align_infos;
    char color[33];
    char bold[33];
    // Labels:
    char title_prefix[33];
    char col_prefix[33];
    char col_block[33];
    char hostname_label[33];
    char user_label[33];
    char uptime_label[33];
    char os_label[33];
    char kernel_label[33];
    char desktop_label[33];
    char shell_label[33];
    char term_label[33];
    char packages_label[33];
    char host_label[33];
    char bios_label[33];
    char cpu_label[33];
    char gpu_label[33];
    char mem_label[33];
    char pub_ip_label[33];
    char loc_ip_label[33];
    char pwd_label[33];
} Config;

extern Config config;

#endif
