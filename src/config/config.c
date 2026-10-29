#include "config.h"
#include "../utils/logos.h"
#include "../utils/return.h"
#include "../utils/utils.h"
#include "../utils/wrappers.h"
#include "parsing.h"

#define GNU_SOURCE

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
#define BOOL_OPTIONS(X) \
    X(align_infos)     X(bat_status)       X(bold)            X(col_background) X(colored_title)   X(cpu_brand)     \
    X(cpu_count)       X(cpu_freq)         X(cpu_temp)        X(desktop_type)   X(desktop_version) X(gpu_brand)     \
    X(kernel_short)    X(kernel_type)      X(loc_docker)      X(loc_localhost)  X(mem_perc)        X(os_arch)       \
    X(pkg_brew)        X(pkg_dpkg)         X(pkg_emerald)     X(pkg_flatpak)    X(pkg_mgr)         X(pkg_pacman)    \
    X(pkg_pip)         X(pkg_rpm)          X(pkg_snap)        X(pwd_path)       X(shell_path)      X(shell_version) \
    X(swap_perc)       X(term_ssh)         X(term_version)

#define PARSE(name) if(parseConfigBool(conf, #name, &buffer) == RET_OK) config.name = buffer;

#define OPTION_ITEM(name) { #name, config.name }
// clang-format on

// This contains the default config values
struct SConfig config = {
    .logo = NULL,
    .color = "",
    .dash = ": ",
    .separator = "-",
    .spacing = 5,

    .gpu_index = 0,
    .date_format = "%02d/%02d/%d %02d:%02d:%02d",
    .col_block_str = "   ",

    .separator_prefix = "",
    .spacing_prefix = "",
    .title_prefix = "",
    .user_prefix = "User",
    .hostname_prefix = "Hostname",
    .uptime_prefix = "Uptime",
    .os_prefix = "OS",
    .kernel_prefix = "Kernel",
    .desktop_prefix = "Desktop",
    .gtk_theme_prefix = "Theme",
    .icon_theme_prefix = "Icons",
    .cursor_theme_prefix = "Cursor",
    .shell_prefix = "Shell",
    .login_shell_prefix = "Login",
    .term_prefix = "Terminal",
    .pkg_prefix = "Packages",
    .host_prefix = "Host",
    .bios_prefix = "BIOS",
    .cpu_prefix = "CPU",
    .gpu_prefix = "GPU",
    .mem_prefix = "Memory",
    .swap_prefix = "Swap",
    .pub_prefix = "Public IP",
    .loc_prefix = "Local IP",
    .pwd_prefix = "Directory",
    .date_prefix = "Date",
    .bat_prefix = "Battery",
    .colors_prefix = "",
    .colors_light_prefix = "",

    .align_infos = false,
    .bold = true,
    .colored_title = true,

    .bat_status = true,
    .col_background = true,
    .cpu_brand = true,
    .cpu_count = true,
    .cpu_freq = true,
    .cpu_temp = false,
    .desktop_type = true,
    .desktop_version = false,
    .gpu_brand = true,
    .kernel_short = false,
    .kernel_type = false,
    .loc_docker = false,
    .loc_localhost = false,
    .mem_perc = true,
    .os_arch = true,
    .pkg_brew = true,
    .pkg_dpkg = true,
    .pkg_emerald = true,
    .pkg_flatpak = true,
    .pkg_mgr = true,
    .pkg_pacman = true,
    .pkg_pip = false,
    .pkg_rpm = true,
    .pkg_snap = true,
    .pwd_path = true,
    .shell_path = false,
    .shell_version = false,
    .swap_perc = true,
    .term_ssh = true,
    .term_version = false,
};

// a return code of 0 means that the option was parsed successfully
static int parseConfigStr(const char *source, const char *field, char *dest, const size_t maxlen) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return ERR_PARSING;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return ERR_PARSING;

    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return ERR_PARSING;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return ERR_PARSING;

    // copies the option
    *end = 0;

    size_t len = strlen(ptr);
    if(len + 1 > maxlen) {
        memcpy(dest, ptr, maxlen);
        dest[maxlen - 1] = 0;
    }
    else
        memcpy(dest, ptr, len + 1);

    *end = '"';

    return RET_OK;
}

// a return code of 0 means that the option was parsed successfully
static int parseConfigInt(const char *source, const char *field, int *dest, const unsigned max) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return ERR_PARSING;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return ERR_PARSING;

    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return ERR_PARSING;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return ERR_PARSING;

    // copies the option
    *end = 0;
    int num = atoi(ptr);
    *end = '"';

    if(num > (int)max)
        return ERR_PARSING;

    *dest = num;

    return RET_OK;
}

// a return code of 0 means that the option was parsed successfully
static int parseConfigBool(const char *source, const char *field, bool *dest) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return ERR_PARSING;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return ERR_PARSING;

    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return ERR_PARSING;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return ERR_PARSING;

    // copies the option
    *end = 0;
    *dest = strcmp(ptr, "false");
    *end = '"';

    return RET_OK;
}

// parse the provided config file
void parseConfig(FILE *fp, SModule *modules, void **ascii_ptr, bool *default_bold, char *default_color, char *default_logo) {
    fseek(fp, 0, SEEK_END);
    size_t len = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *conf = malloc(len);
    if(conf == NULL) {
        perror("malloc");
        return;
    }
    size_t read = fread(conf, sizeof(*conf), len, fp);
    fclose(fp);
    if(read > 0)
        conf[read - 1] = 0;
    else {
        fprintf(stderr, "fread() failed: %zu\n", read);
        return;
    }

    // used later
    char *ptr, *ptr2;

    // remove comments
    uncomment(conf, ';');
    uncomment(conf, '#');

    // handle escape sequences
    unescape(conf);

    // GENERAL OPTIONS

    // ascii art
    char path[96] = "";
    parseConfigStr(conf, "ascii_art", path, sizeof(path));
    if(path[0])
        *ascii_ptr = fileToLogo(path);

    // logo
    char logo[32] = "";
    parseConfigStr(conf, "logo", logo, sizeof(logo));
    if(logo[0]) {
        for(size_t i = 0; i < sizeof(logos) / sizeof(logos[0]); ++i)
            if(strcmp(logos[i][0], logo) == 0) {
                config.logo = logos[i];
                safeStrncpy(default_logo, logos[i][0], 16);
                safeStrncpy(config.color, logos[i][1], sizeof(config.color));
            }
    }

    // color
    char color[16] = "";
    parseConfigStr(conf, "default_color", color, sizeof(color));
    if(color[0]) {
        const char *colors[][2] = {
            {"black", "\033[30m"},  {"red", "\033[31m"},  {"green", "\033[32m"}, {"yellow", "\033[33m"}, {"blue", "\033[34m"},
            {"purple", "\033[35m"}, {"cyan", "\033[36m"}, {"gray", "\033[90m"},  {"white", "\033[37m"},
        };

        for(int i = 0; i < 9; ++i)
            if(strcmp(color, *colors[i]) == 0) {
                strcpy(config.color, colors[i][1]);
                strcpy(default_color, colors[i][1]);
            }
    }

    // dash
    parseConfigStr(conf, "dash", config.dash, sizeof(config.dash));

    // spacing
    parseConfigInt(conf, "spacing", &config.spacing, 64);

    // separator
    parseConfigStr(conf, "separator_character", config.separator, sizeof(config.separator));

    // BOOLEAN OPTIONS (check utils/utils.h)

    bool buffer;
    BOOL_OPTIONS(PARSE)

    *default_bold = config.bold;

    // OTHER MODULE-RELATED OPTIONS

    parseConfigInt(conf, "gpu_index", &config.gpu_index, 3);

    parseConfigStr(conf, "date_format", config.date_format, sizeof(config.date_format));

    parseConfigStr(conf, "col_block_str", config.col_block_str, sizeof(config.col_block_str));

    // LABELS
    char *prefixes[][2] = {
        OPTION_ITEM(bat_prefix),          OPTION_ITEM(bios_prefix),      OPTION_ITEM(colors_light_prefix), OPTION_ITEM(colors_prefix),  OPTION_ITEM(cpu_prefix),
        OPTION_ITEM(cursor_theme_prefix), OPTION_ITEM(date_prefix),      OPTION_ITEM(desktop_prefix),      OPTION_ITEM(gpu_prefix),     OPTION_ITEM(gtk_theme_prefix),
        OPTION_ITEM(host_prefix),         OPTION_ITEM(hostname_prefix),  OPTION_ITEM(icon_theme_prefix),   OPTION_ITEM(kernel_prefix),  OPTION_ITEM(loc_prefix),
        OPTION_ITEM(login_shell_prefix),  OPTION_ITEM(mem_prefix),       OPTION_ITEM(os_prefix),           OPTION_ITEM(pkg_prefix),     OPTION_ITEM(pub_prefix),
        OPTION_ITEM(pwd_prefix),          OPTION_ITEM(separator_prefix), OPTION_ITEM(shell_prefix),        OPTION_ITEM(spacing_prefix), OPTION_ITEM(swap_prefix),
        OPTION_ITEM(term_prefix),         OPTION_ITEM(title_prefix),     OPTION_ITEM(uptime_prefix),       OPTION_ITEM(user_prefix),
    };

    for(size_t i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i)
        parseConfigStr(conf, prefixes[i][0], prefixes[i][1], 64);

    // MODULES

    ptr = strstr(conf, "modules");
    if(ptr == NULL) {
        free(conf);
        return;
    }

    ptr2 = strchr(ptr, '{');
    if(ptr2 == NULL) {
        free(conf);
        return;
    }

    char *end = strchr(ptr2, '}');
    if(end == NULL) {
        free(conf);
        return;
    }
    *end = 0;

    while((ptr = strchr(ptr2, '"'))) {
        ++ptr;

        ptr2 = strchr(ptr, '"');
        if(ptr2 == NULL) {
            free(conf);
            return;
        }
        *ptr2 = 0;

        addModule(modules, ptr);

        // *ptr2 = '"';
        ++ptr2;
    }

    // *end = '}';

    free(conf);
}
