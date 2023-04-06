#include <string.h>
#include <stdio.h>

#include "logos.h"
#include "utils.h"

// print a certain line of the logo
void get_logo_line(char *dest, unsigned *line) {
    if(!config.logo || !dest || !line)
        return;

    if(config.logo[*line])
        strcat(dest, config.logo[(*line)++]);
    else
        strcat(dest, config.logo[2]);
}

// print no more than maxlen visible characters of line
void print_line(char *line, const size_t maxlen) {
    if(config.bold)
        fputs("\033[1m", stdout);

    bool escaping = false;
    int unicode = 0;

    for(size_t i = 0, len = 0; len < maxlen && i < strlen(line); ++i) {
            putc(line[i], stdout);

            if(line[i] == '\n')
                break;
            else if(line[i] == '\033')
                escaping = true;
            else if(line[i] < 0) {
                if(line[i] & 0x40) {
                    if(line[i] & 0x20) {
                        if(line[i] & 0x10)
                            unicode = 3;    // first of 4 unicode bytes (0b11110xxx)
                        else
                            unicode = 2;    // first of 3 unicode bytes (0b1110xxxx)
                    }
                    else
                        unicode = 1;        // first of 2 unicode bytes (0b110xxxxx)
                }
                else if(unicode-- == 1)     // unicode continuation byte (0b10xxxxxx)
                    ++len;
            }
            else {
                // look mom, I just wanted to try to write some branchless code

                // this line is a bit weird
                // ++len <=> escaping == 0
                len += 1-escaping;

                /* m is found and escaping => escaping = 0
                 * m is found and not escaping => escaping = 0
                 * m is not found and escaping => escaping = 1
                 * m is found and not escaping => escaping = 0
                 */
                escaping = (line[i] != 'm') && escaping;
            }
        }

    fputs("\n\033[0m", stdout);
}

// a return code of 0 means that the option was parsed successfully
int parse_config_str(const char* source, const char *field, char *dest, const size_t maxlen) {
    char *ptr;
    char *end;

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
    end = strchr(ptr, '"');
    if(!end)
        return 1;

    // copies the option
    *end = 0;

    // doing this instead of strncpy because I get some weird behaivour using that
    size_t len = strlen(ptr);
    memcpy(dest, ptr, len > maxlen ? maxlen : len);
    *end = '"';

    return 0;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_int(const char *source, const char *field, int *dest, const int max) {
    char *ptr;
    char *end;

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
    end = strchr(ptr, '"');
    if(!end)
        return 1;

    // copies the option
    *end = 0;
    int num = atoi(ptr);
    *end = '"';

    if((unsigned)num > (unsigned)max)
        return 1;

    *dest =  num;

    return 0;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_bool(const char *source, const char *field, bool *dest) {
    char *ptr;
    char *end;

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
    end = strchr(ptr, '"');
    if(!end)
        return 1;

    // copies the option
    *end = 0;
    *dest = !strcmp(ptr, "true");
    *end = '"';

    return 0;
}

// output is 1 if there's a user error.
void parse_config(const char *file, bool *default_bold, char *default_color, char *default_logo) {
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
    parse_config_bool(conf, "align_infos", &config.align);
    
    // bold
    parse_config_bool(conf, "bold", &config.bold);
    *default_bold = config.bold;

    // logo
    char logo[32] = "";
    parse_config_str(conf, "logo", logo, sizeof(logo));
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
    parse_config_str(conf, "default_color", color, sizeof(color));
    if(color[0]) {
        char *colors[9][2] = {
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

        for(int i = 0; i < 9; ++i)
            if(!strcmp(color, *colors[i])) {
                strcpy(config.color, colors[i][1]);
                strcpy(default_color, colors[i][1]);
            }
    }

    // dash
    parse_config_str(conf, "dash", config.dash, sizeof(config.dash));

    // spacing
    parse_config_int(conf, "spacing", &config.spacing, 64);

    // separator
    parse_config_str(conf, "separator", config.separator, sizeof(config.separator));

    // MODULE-SPECIFIC OPTIONS

    struct BooleanOption {
        bool *config_option;
        char *config_name;
    };
    struct BooleanOption options[] = {
    //  {&config.config_option, "config_name"}
        {&config.title_color,"colored_title"},
        {&config.os_arch,"os_arch"},
        {&config.kernel_short, "kernel_short"},
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
        parse_config_bool(conf, options[i].config_name, options[i].config_option);

    parse_config_int(conf, "col_block_len", &config.col_block_len, 16);

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
        {config.light_colors_prefix, "colors_prefix_light"},
    };

    for(size_t i = 0; i < sizeof(modules)/sizeof(modules[0]); ++i)
        parse_config_str(conf, modules[i][1], modules[i][0], sizeof(modules[i][0]));

    free(conf);
}

// check every '\\' in str and unescape "\\\\" "\\n" "\\033"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        switch(str[1]) {
            case '\\':
                memmove(str, str+1, strlen(str));
                break;
            case 'e':
                memmove(str, str+1, strlen(str));
                *str = '\033';
                break;
            case '0':
                if(str[2] == '3' && str[3] == '3') {
                    memmove(str, str+3, strlen(str));
                    *str = '\033';
                }
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

size_t strlen_real(const char *str) {
    size_t len = 0;

    bool escaping = false;

    // determine how long the printed string is (same logic as in print_line, utils.c)
    while(*str) {
        if(*str == '\n')
            break;

        // unicode continuation byte 0x10xxxxxx
        if(*str & 0x80 && !(*str & 0x40)) {
            ++str;
            continue;
        }

        if(*str != '\033') {
            len += 1-escaping;

            escaping = (*str != 'm') && escaping;
        }
        else
            escaping = true;
        
        ++str;
    }

    return len;
}

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest){(void)dest;return 1;}
int spacing(char *dest){(void)dest;return 1;}
int title(char *dest){(void)dest;return 1;}
