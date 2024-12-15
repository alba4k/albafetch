#pragma once

#include "../utils/utils.h"

#include <stdbool.h>
#include <stdint.h>

struct Config {
    /* Starting from the least significant byte, see the #define statements later
    * 0. _align
    * 1. _bold
    * 2. _title_color
    * 3. _os_arch
    * 4. _kernel_short
    * 5. de_type
    * 6. shell_path
    * 7. term_ssh
    * 8. pkg_mgr
    * 9. pkg_pacman
    * 10. pkg_dpkg
    * 11. pkg_rpm
    * 12. pkg_flatpak
    * 13. pkg_snap
    * 14. pkg_brew
    * 15. pkg_pip
    * 16. cpu_brand
    * 17. cpu_freq
    * 18. cpu_count
    * 19. gpu_brand
    * 20. mem_perc
    * 21. loc_localhost
    * 22. loc_docker
    * 23. pwd_path
    * 24. col_background
    * 25. bat_status
    * 26. [...]
    */
    uint64_t boolean_options;

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
    char pub_prefix[64];
    char loc_prefix[64];
    char pwd_prefix[64];
    char date_prefix[64];
    char bat_prefix[64];
    char colors_prefix[64];
    char light_colors_prefix[64];
};
extern struct Config config;

#define _align           config.boolean_options & 0x1
#define _bold            config.boolean_options & 0x2
#define _title_color     config.boolean_options & 0x4
#define _os_arch         config.boolean_options & 0x8
#define _kernel_short    config.boolean_options & 0x10
#define _de_type         config.boolean_options & 0x20
#define _shell_path      config.boolean_options & 0x40
#define _term_ssh        config.boolean_options & 0x80
#define _pkg_mgr         config.boolean_options & 0x100
#define _pkg_pacman      config.boolean_options & 0x200
#define _pkg_dpkg        config.boolean_options & 0x400
#define _pkg_rpm         config.boolean_options & 0x800
#define _pkg_flatpak     config.boolean_options & 0x1000
#define _pkg_snap        config.boolean_options & 0x2000
#define _pkg_brew        config.boolean_options & 0x4000
#define _pkg_pip         config.boolean_options & 0x8000
#define _cpu_brand       config.boolean_options & 0x10000
#define _cpu_freq        config.boolean_options & 0x20000
#define _cpu_count       config.boolean_options & 0x40000
#define _gpu_brand       config.boolean_options & 0x80000
#define _mem_perc        config.boolean_options & 0x100000
#define _loc_localhost   config.boolean_options & 0x200000
#define _loc_docker      config.boolean_options & 0x400000
#define _pwd_path        config.boolean_options & 0x800000
#define _kernel_type     config.boolean_options & 0x1000000
#define _col_background  config.boolean_options & 0x2000000
#define _bat_status      config.boolean_options & 0x4000000

void parse_config(const char *file, struct Module *modules, void **ascii_ptr, bool *default_bold, char *default_color, char *default_logo);
