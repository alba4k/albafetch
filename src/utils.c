#include <string.h>
#include <stdio.h>

#include "logos.h"
#include "utils.h"

void print_line(char **logo, unsigned *line, size_t maxlen) {
    if(!logo)
        return;

    int escaping = 0;

    if(*line > 3)
        putc('\n', stdout);

    printf("\e[0m%s", config.bold ? "\e[1m" : "");

    if(!logo[*line])
        for(size_t i = 0, len = 0; len < maxlen && i < strlen(logo[2]); ++i) {
            putc(logo[2][i], stdout);

            if(logo[2][i] != '\e') {
                // look mom, I just wanted to try to write some branchless code

                // this line is a bit weird
                // ++len <=> escaping == 0
                len += 1-escaping;

                /* m is found and escaping => escaping = 0
                 * m is found and not escaping => escaping = 0
                 * m is not found and escaping => escaping = 1
                 * m is found and not escaping => escaping = 0
                 */

                escaping = (logo[2][i] != 'm') && escaping;
            }
            else
                escaping = 1;
        }
    else
        for(size_t i = 0, len = 0; len < maxlen && i < strlen(logo[*line]); ++i) {
            putc(logo[*line][i], stdout);

            if(logo[*line][i] != '\e') {
                // same as above

                len += 1-escaping;

                escaping = (logo[*line][i] != 'm') && escaping;
            }
            else
                escaping = 1;
        }
        
    fputs(config.color, stdout);
    for(int i = 0; i < 4 && ((long)(maxlen - strlen(logo[2]) - i) > 0); ++i)
        putc(' ', stdout);
    
    ++(*line);
}

// check every '\\' in str and unescape "\\\\" "\\n" "\\e"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        switch(str[1]) {
            case '\\':
                memmove(str, str+1, strlen(str));
                break;
            case 'e':
                memmove(str, str+1, strlen(str));
                *str = '\e';
                break;
            case 'n':
                memmove(str, str+1, strlen(str));
                *str = '\n';
                break;
        }
        ++str;
    }
}

// stuff needed for libcurl
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL)
        return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_str(char* source, char *dest, char *field, size_t maxlen) {
    char *ptr;

    // looks for the keyword
    ptr = strstr(source, field);
    if(!ptr)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(!ptr)
        return 1;

    // checks whether the string continues after
    ++ptr;
    if(!(*ptr))
        return 1;

    // looks for the closing "
    field = strchr(ptr, '"');
    if(!field)
        return 1;

    // copies the option
    *field = 0;
    strncpy(dest, ptr, maxlen-1);
    *field = '"';

    return 0;
}

// in the following, a return code of 1 means success
int parse_config_int(char *source, int *dest, char *field) {
    char *ptr;

    // looks for the keyword
    ptr = strstr(source, field);
    if(!ptr)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(!ptr)
        return 1;
        
    // checks whether the string continues after
    ++ptr;
    if(!(*ptr))
        return 1;

    // looks for the closing "
    field = strchr(ptr, '"');
    if(!field)
        return 1;

    // copies the option
    *field = 0;
    *dest = atoi(ptr);
    *field = '"';

    return 0;
}

// in the following, a return code of 1 means success
int parse_config_bool(char *source, bool *dest, char *field) {
    char *ptr;

    // looks for the keyword
    ptr = strstr(source, field);
    if(!ptr)
        return 1;

    // looks for the opening "
    ptr = strchr(ptr, '"');
    if(!ptr)
        return 1;
        
    // checks whether the string continues after
    ++ptr;
    if(!(*ptr))
        return 1;

    // looks for the closing "
    field = strchr(ptr, '"');
    if(!field)
        return 1;

    // copies the option
    *field = 0;
    *dest = !strcmp(ptr, "true");
    *field = '"';

    return 0;
}

// output is 1 if there's a user error.
void parse_config(char *file, bool *default_bold, char *default_color, char *default_logo) {
    // really bad code here, you don't need to look

    FILE *fp = fopen(file, "r");

    if(!fp)
        return;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    
    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    unescape(conf);

    // remove comments
    char *ptr = conf, *ptr2;
    while((ptr = strchr(ptr, ';'))) {
        ptr2 = strchr(ptr, '\n');
        if(!ptr)
            break;

        memmove(ptr, ptr2+1, strlen(ptr2+1)+1);
    }
    ptr = conf;
    while((ptr = strchr(ptr, '#'))) {
        ptr2 = strchr(ptr, '\n');
        if(!ptr)
            break;

        memmove(ptr, ptr2+1, strlen(ptr2+1)+1);
    }

    // GENERAL OPTIONS

    // align_infos
    parse_config_bool(conf, &config.align, "align_infos");
    
    // bold
    parse_config_bool(conf, &config.bold, "bold");
    *default_bold = config.bold;

    // logo
    char logo[32] = "";
    parse_config_str(conf, logo, "logo", 32);
    if(logo[0]) {
        for(size_t i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
            if(!strcmp(logos[i][0], logo)) {
                config.logo = (char**)logos[i];
                strcpy(default_logo, logos[i][0]);
                strcpy(config.color, logos[i][1]);
            }
    }

    // color
    char color[16] = "";
    parse_config_str(conf, color, "default_color", 16);
    if(color[0]) {
        char *colors[9][2] = {
            {"black", "\e[30m"},
            {"red", "\e[31m"},
            {"green", "\e[32m"},
            {"yellow", "\e[33m"},
            {"blue", "\e[34m"},
            {"purple", "\e[35m"},
            {"cyan", "\e[36m"},
            {"gray", "\e[90m"},
            {"white", "\e[37m"},
        };

        for(int i = 0; i < 9; ++i)
            if(!strcmp(color, *colors[i])) {
                strcpy(config.color, colors[i][1]);
                strcpy(default_color, colors[i][1]);
            }
    }

    // dash
    parse_config_str(conf, config.dash, "dash", 16);

    // MODULE-SPECIFIC OPTIONS

    struct BooleanOption {
        bool *config_option;
        char *config_name;
    };
    struct BooleanOption options[] = {
    //  {&config.config_option, "config_name"}
        {&config.title_color,"colored_title"},
        {&config.os_arch,"os_arch"},
        {&config.de_type,"desktop_type"},
        {&config.shell_path,"shell_path"},
        {&config.cpu_brand,"cpu_brand"},
        {&config.cpu_freq,"cpu_freq"},
        {&config.cpu_count,"cpu_count"},
        {&config.gpu_brand,"gpu_brand"},
        {&config.mem_perc,"mem_perc"},
        {&config.pkg_mgr,"pkg_mgr"},
        {&config.pkg_pacman,"pkg_pacman"},
        {&config.pkg_dpkg,"pkg_dpkg"},
        {&config.pkg_rpm,"pkg_rpm"},
        {&config.pkg_flatpak,"pkg_flatpak"},
        {&config.pkg_snap,"pkg_snap"},
        {&config.pkg_pip,"pkg_pip"},
        {&config.pkg_brew,"pkg_brew"},
        {&config.loc_localdomain,"loc_localdomain"},
        {&config.loc_docker,"loc_docker"},
    };

    for(size_t i = 0; i < sizeof(options)/sizeof(options[0]); ++i)
        parse_config_bool(conf, options[i].config_option, options[i].config_name);

    parse_config_int(conf, &config.col_block_len, "col_block_len");

    // LABELS

    char *modules[][2] = {
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
        {config.shell_prefix, "shell_prefix"},
        {config.login_shell_prefixix, "login_shell_prefixix"},
        {config.term_prefix, "term_prefix"},
        {config.packages_prefix, "packages_prefix"},
        {config.host_prefix, "host_prefix"},
        {config.bios_prefix, "bios_prefix"},
        {config.cpu_prefix, "cpu_prefix"},
        {config.gpu_prefix, "gpu_prefix"},
        {config.mem_prefix, "mem_prefix"},
        {config.public_ip_prefix, "public_ip_prefix"},
        {config.loc_prefix, "loc_prefix"},
        {config.pwd_prefix, "pwd_prefix"},
        {config.date_prefix, "date_prefix"},
        {config.colors_prefix, "colors_prefix"},
        {config.light_colors_prefix, "light_colors_prefix"},
    };

    for(size_t i = 0; i < sizeof(modules)/sizeof(modules[0]); ++i)
        parse_config_str(conf, modules[i][0], modules[i][1], 64);

    free(conf);
}

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest){(void)dest;return 1;}
int spacing(char *dest){(void)dest;return 1;}
int title(char *dest){(void)dest;return 1;}
