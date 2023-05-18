#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct Config {
    char **logo;
    char color[8];
    char dash[16];
    char separator[8];
    int spacing;

    char date_format[32];
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
    char colors_prefix[64];
    char light_colors_prefix[64];
};
extern struct Config config;


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
 * 21. loc_localdomain
 * 22. loc_docker
 * 23. pwd_path
 * 24. [...]
 */
extern uint64_t options;

#define align           options & 0x1
#define bold            options & 0x2
#define title_color     options & 0x4
#define os_arch         options & 0x8
#define kernel_short    options & 0x10
#define de_type         options & 0x20
#define shell_path      options & 0x40
#define term_ssh        options & 0x80
#define pkg_mgr         options & 0x100
#define pkg_pacman      options & 0x200
#define pkg_dpkg        options & 0x400
#define pkg_rpm         options & 0x800
#define pkg_flatpak     options & 0x1000
#define pkg_snap        options & 0x2000
#define pkg_brew        options & 0x4000
#define pkg_pip         options & 0x8000
#define cpu_brand       options & 0x10000
#define cpu_freq        options & 0x20000
#define cpu_count       options & 0x40000
#define gpu_brand       options & 0x80000
#define mem_perc        options & 0x100000
#define loc_localdomain options & 0x200000
#define loc_docker      options & 0x400000
#define pwd_path        options & 0x800000

// needed for libcurl
struct MemoryStruct {
    char *memory;
    size_t size;
};

void get_logo_line(char *dest, unsigned *line);

void print_line(char *line, const size_t maxlen);

void parse_config(const char *file, bool *default_bold, char *default_color, char *default_logo);

void unescape(char *str);

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

size_t strlen_real(const char *str);

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest);
int spacing(char *dest);
int title(char *dest);

#endif // UTILS_H