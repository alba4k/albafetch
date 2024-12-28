#include "config.h"
#include "parsing.h"
#include "../logos.h"
#include "../utils/utils.h"

#define _GNU_SOURCE

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// a return code of 0 means that the option was parsed successfully
int parse_config_str(const char* source, const char *field, char *dest, const size_t maxlen) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return 1;

    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return 1;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return 1;

    // copies the option
    *end = 0;
    
    size_t len = strlen(ptr);
    if(len+1 > maxlen) {
        memcpy(dest, ptr, maxlen);
        dest[maxlen-1] = 0;
    }
    else
dest[maxlen-1] = '\0';  // Ensure null termination


    *end = '"';

    return 0;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_int(const char *source, const char *field, int *dest, const unsigned max) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return 1;
        
    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return 1;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return 1;

    // copies the option
    *end = 0;
    int num = atoi(ptr);
    *end = '"';

    if((unsigned)num > max)
        return 1;

    *dest =  num;

    return 0;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_bool(const char *source, const char *field, bool *dest) {
    char *ptr, *end;

    // looks for the keyword
    ptr = strstr(source, field);
    if(ptr == NULL)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(ptr == NULL)
        return 1;
        
    // checks whether the string continues after
    ++ptr;
    if(*ptr == 0)
        return 1;

    // looks for the closing "
    end = strchr(ptr, '"');
    if(end == NULL)
        return 1;

    // copies the option
    *end = 0;
    *dest = strcmp(ptr, "false");
    *end = '"';

    return 0;
}

// parse the provided config file
void parse_config(const char *file, struct Module *modules, void **ascii_ptr, bool *default_bold, char *default_color, char *default_logo) {
    FILE *fp = fopen(file, "r");

    if(fp == NULL)
        return;

    fseek(fp, 0, SEEK_END);
    size_t len = (size_t)ftell(fp);
    rewind(fp);
    
    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    // used later
    char *ptr, *ptr2;

    // remove comments
    uncomment(conf, '#');
    uncomment(conf, ';');

    // handle escape sequences
    unescape(conf);

    // GENERAL OPTIONS

    // ascii art
    char path[96] = "";
    parse_config_str(conf, "ascii_art", path, sizeof(path));
    if(path[0])
        *ascii_ptr = file_to_logo(path);
    
    // logo
    char logo[32] = "";
    parse_config_str(conf, "logo", logo, sizeof(logo));
    if(logo[0]) {
        for(size_t i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
            if (strcmp(logos[i][0], logo) == 0) {
                config.logo = logos[i];

                // Ensure that 'default_logo' and 'config.color' have enough space for copying
                strncpy(default_logo, logos[i][0], sizeof(default_logo) - 1);
                default_logo[sizeof(default_logo) - 1] = '\0';  // Ensure null-termination

                strncpy(config.color, logos[i][1], sizeof(config.color) - 1);
                config.color[sizeof(config.color) - 1] = '\0';  // Ensure null-termination
            }
    // color
    char color[16] = "";
    parse_config_str(conf, "default_color", color, sizeof(color));
    if(color[0]) {
        const char *colors[][2] = {
            {"black", "\033[30m"},
            {"red", "\033[31m"},
            {"green", "\033[32m"},
            {"yellow", "\033[33m"},
            {"blue", "\033[34m"},
            {"purple", "\033[35m"},
            {"cyan", "\033[36m"},
            {"gray", "\033[90m"},
            {"white", "\033[37m"},
        };

        for (int i = 0; i < 9; ++i)
            if (strcmp(color, *colors[i]) == 0) {
                // Use strncpy to safely copy the color string, ensuring no overflow
                strncpy(config.color, colors[i][1], sizeof(config.color) - 1);
                config.color[sizeof(config.color) - 1] = '\0';  // Ensure null-termination

                strncpy(default_color, colors[i][1], sizeof(default_color) - 1);
                default_color[sizeof(default_color) - 1] = '\0';  // Ensure null-termination
            }


    // dash
    parse_config_str(conf, "dash", config.dash, sizeof(config.dash));

    // spacing
    parse_config_int(conf, "spacing", &config.spacing, 64);

    // separator
    parse_config_str(conf, "separator_character", config.separator, sizeof(config.separator));

    // BOOLEAN OPTIONS (check utils/utils.h)

    const char *booleanOptions[] = {
        "align_infos",
        "bold",
        "colored_title",
        "os_arch",
        "kernel_short",
        "desktop_type",
        "shell_path",
        "term_ssh",
        "pkg_mgr",
        "pkg_pacman",
        "pkg_dpkg",
        "pkg_rpm",
        "pkg_flatpak",
        "pkg_snap",
        "pkg_brew",
        "pkg_pip",
        "cpu_brand",
        "cpu_freq",
        "cpu_count",
        "gpu_brand",
        "mem_perc",
        "loc_localhost",
        "loc_docker",
        "pwd_path",
        "kernel_type",
        "col_background",
        "bat_status"
    };

    bool buffer;
    for(size_t i = 0; i < sizeof(booleanOptions)/sizeof(booleanOptions[0]); ++i) {
        if(parse_config_bool(conf, booleanOptions[i], &buffer) == 0) {
            if(buffer)
                config.boolean_options |= ((uint64_t)1 << i);
            else
                config.boolean_options &= ~((uint64_t)1 << i);
        }
    }
    *default_bold = _bold;

    // OTHER MODULE-RELATED OPTIONS

    parse_config_int(conf, "gpu_index", &config.gpu_index, 3);

    parse_config_str(conf, "date_format", config.date_format, sizeof(config.date_format));

    parse_config_str(conf, "col_block_str", config.col_block_str, sizeof(config.col_block_str));

    // LABELS

    struct Prefix {
        char *option;
        const char *config_name;
    };
    
    struct Prefix prefixes[] = {
    //  {config.module_prefix, "module_prefix"}
        {config.separator_prefix, "separator_prefix"},
        {config.spacing_prefix, "spacing_prefix"},
        {config.title_prefix, "title_prefix"},
        {config.user_prefix, "user_prefix"},
        {config.hostname_prefix, "hostname_prefix"},
        {config.uptime_prefix, "uptime_prefix"},
        {config.os_prefix, "os_prefix"},
        {config.kernel_prefix, "kernel_prefix"},
        {config.desktop_prefix, "desktop_prefix"},
        {config.gtk_theme_prefix, "gtk_theme_prefix"},
        {config.icon_theme_prefix, "icon_theme_prefix"},
        {config.cursor_theme_prefix, "cursor_theme_prefix"},
        {config.shell_prefix, "shell_prefix"},
        {config.login_shell_prefix, "login_shell_prefix"},
        {config.term_prefix, "term_prefix"},
        {config.pkg_prefix, "pkg_prefix"},
        {config.host_prefix, "host_prefix"},
        {config.bios_prefix, "bios_prefix"},
        {config.cpu_prefix, "cpu_prefix"},
        {config.gpu_prefix, "gpu_prefix"},
        {config.mem_prefix, "mem_prefix"},
        {config.pub_prefix, "pub_prefix"},
        {config.loc_prefix, "loc_prefix"},
        {config.pwd_prefix, "pwd_prefix"},
        {config.date_prefix, "date_prefix"},
        {config.bat_prefix, "bat_prefix"},
        {config.colors_prefix, "colors_prefix"},
        {config.light_colors_prefix, "colors_light_prefix"},
    };

    for(size_t i = 0; i < sizeof(prefixes)/sizeof(prefixes[0]); ++i)
        parse_config_str(conf, prefixes[i].config_name, prefixes[i].option, 64);

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

        add_module(modules, ptr);

     // *ptr2 = '"';
        ++ptr2;
    }

 // *end = '}';

    free(conf);
}
