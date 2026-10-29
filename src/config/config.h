#pragma once

#include "../utils/info_module.h"

#include <stdbool.h>
#include <stdio.h>

struct SConfig {
    char **logo;
    char color[8];
    char dash[16];
    char separator[8];
    int spacing;

    int gpu_index;
    char date_format[32];
    char col_block_str[24];

    char separator_prefix[64];
    char spacing_prefix[64];
    char title_prefix[64];
    char user_prefix[64];
    char hostname_prefix[64];
    char uptime_prefix[64];
    char os_prefix[64];
    char kernel_prefix[64];
    char desktop_prefix[64];
    char gtk_theme_prefix[64];
    char icon_theme_prefix[64];
    char cursor_theme_prefix[64];
    char shell_prefix[64];
    char login_shell_prefix[64];
    char term_prefix[64];
    char pkg_prefix[64];
    char host_prefix[64];
    char bios_prefix[64];
    char cpu_prefix[64];
    char gpu_prefix[64];
    char mem_prefix[64];
    char swap_prefix[64];
    char pub_prefix[64];
    char loc_prefix[64];
    char pwd_prefix[64];
    char date_prefix[64];
    char bat_prefix[64];
    char colors_prefix[64];
    char colors_light_prefix[64];

    bool align_infos : 1;
    bool bold : 1;
    bool colored_title : 1;

    bool bat_status : 1;
    bool col_background : 1;
    bool cpu_brand : 1;
    bool cpu_count : 1;
    bool cpu_freq : 1;
    bool cpu_temp : 1;
    bool desktop_type : 1;
    bool desktop_version : 1;
    bool gpu_brand : 1;
    bool kernel_short : 1;
    bool kernel_type : 1;
    bool loc_docker : 1;
    bool loc_localhost : 1;
    bool mem_perc : 1;
    bool os_arch : 1;
    bool pkg_brew : 1;
    bool pkg_dpkg : 1;
    bool pkg_emerald : 1;
    bool pkg_flatpak : 1;
    bool pkg_mgr : 1;
    bool pkg_pacman : 1;
    bool pkg_pip : 1;
    bool pkg_rpm : 1;
    bool pkg_snap : 1;
    bool pwd_path : 1;
    bool shell_path : 1;
    bool shell_version : 1;
    bool swap_perc : 1;
    bool term_ssh : 1;
    bool term_version : 1;
};
extern struct SConfig config;

void parseConfig(FILE *fp, SModule *modules, void **ascii_ptr, bool *default_bold, char *default_color, char *default_logo);
