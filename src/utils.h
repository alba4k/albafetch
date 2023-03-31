#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

struct Config {
    bool align;
    bool bold;
    char **logo;
    char color[8];
    char dash[16];

    bool title_color;
    bool os_arch;
    bool de_type;
    bool shell_path;
    bool cpu_brand;
    bool cpu_freq;
    bool cpu_count;
    bool gpu_brand;
    bool mem_perc;
    bool pkg_mgr;
    bool pkg_pacman;
    bool pkg_dpkg;
    bool pkg_rpm;
    bool pkg_flatpak;
    bool pkg_snap;
    bool pkg_pip;
    bool pkg_brew;
    bool loc_localdomain;
    bool loc_docker;
    int col_block_len;

    char separator_prefix[64];
    char spacing_prefix[64];
    char title_prefix[64];
    char user_prefix[64];
    char hostname_prefix[64];
    char uptime_prefix[64];
    char os_prefix[64];
    char kernel_prefix[64];
    char desktop_prefix[64];
    char shell_prefix[64];
    char login_shell_prefixix[64];
    char term_prefix[64];
    char packages_prefix[64];
    char host_prefix[64];
    char bios_prefix[64];
    char cpu_prefix[64];
    char gpu_prefix[64];
    char mem_prefix[64];
    char public_ip_prefix[64];
    char loc_prefix[64];
    char pwd_prefix[64];
    char date_prefix[64];
    char colors_prefix[64];
    char light_colors_prefix[64];
};
extern struct Config config;

void print_line(char **logo, unsigned *line, size_t maxlen);

void unescape(char *str);

void parse_config(char *file, bool *default_bold, char *default_color, char *default_logo);

// needed for libcurl
struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest);
int spacing(char *dest);
int title(char *dest);

#endif // UTILS_H