#pragma once

#ifndef UTILS_H
#define UTILS_H

#define _GNU_SOURCE

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct Config {
    /* Starting from the least significant byte, see the #define statements later
    * 0. align
    * 1. bold
    * 2. title_color
    * 3. os_arch
    * 4. kernel_short
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
    * 25. [...]
    */
    uint64_t options;

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

#define align           config.options & 0x1
#define bold            config.options & 0x2
#define title_color     config.options & 0x4
#define os_arch         config.options & 0x8
#define kernel_short    config.options & 0x10
#define de_type         config.options & 0x20
#define shell_path      config.options & 0x40
#define term_ssh        config.options & 0x80
#define pkg_mgr         config.options & 0x100
#define pkg_pacman      config.options & 0x200
#define pkg_dpkg        config.options & 0x400
#define pkg_rpm         config.options & 0x800
#define pkg_flatpak     config.options & 0x1000
#define pkg_snap        config.options & 0x2000
#define pkg_brew        config.options & 0x4000
#define pkg_pip         config.options & 0x8000
#define cpu_brand       config.options & 0x10000
#define cpu_freq        config.options & 0x20000
#define cpu_count       config.options & 0x40000
#define gpu_brand       config.options & 0x80000
#define mem_perc        config.options & 0x100000
#define loc_localhost   config.options & 0x200000
#define loc_docker      config.options & 0x400000
#define pwd_path        config.options & 0x800000
#define kernel_type     config.options & 0x1000000
#define col_background  config.options & 0x2000000
#define bat_status      config.options & 0x4000000

// element of a module linked list
struct Module {
    char *id;               // module identifier
    char *label;            // module label
    int (*func)(char *);    // function to run
    struct Module *next;    // next module
};

void *file_to_logo(char *file);

void add_module(struct Module *array, char *id);

void destroy_array(struct Module *array);

void get_logo_line(char *dest, unsigned *line);

void print_line(char *line, const size_t maxlen);

void parse_config(const char *file, struct Module *modules, void **ascii_ptr, bool *default_bold, char *default_color, char *default_logo);

void unescape(char *str);

int exec_cmd(char *buf, size_t len, char *const *argv);

size_t strlen_real(const char *str);

#endif // UTILS_H
