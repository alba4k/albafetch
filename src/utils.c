#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "logos.h"
#include "utils.h"

// copy an ascii art from file to mem
int file_to_logo(char *file, char *mem) {
    FILE *fp = fopen(file, "r");
    if(!fp)
        return 1;

    /* mem is assumed to be a 10 KiB buffer, aka 10240 B.
     * this will be filled in with up to 64 lines,
     * each of which can be up to 160 bytes long.
     * (LINE_LEN * LINE_NUM) should equal this size.
     * Check out main.c, line 132
    */
    #define LINE_LEN 256
    #define LINE_NUM 40

    // where the final logo is saved
    static char *logo[LINE_NUM + 1];

    char *buffer = NULL;
    size_t len = 0;
    size_t line_len;
    int i = 1;

    // setting the correct color (or eventually the first line)
    line_len = getline(&buffer, &len, fp);  // save the first line to buffer
    if(line_len != (size_t)-1) {            // getline returns -1 in case of error
        if(buffer[line_len-1] == '\n')
            buffer[line_len-1] = 0;

        config.color[0] = 0;

        char *colors[][2] = {
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

        for(int j = 0; j < 9; ++j)
            if(!strcmp(buffer, *colors[j]))
                strcpy(config.color, colors[j][1]);
            
        if(!config.color[0]) {
            unescape(buffer);

            logo[i+2] = mem + i*LINE_LEN;
            strncpy(logo[i+2], buffer, LINE_LEN);

            ++i;
        }
    }
    else
        return 1;

    // for every remaining line of the logo...
    while((line_len = getline(&buffer, &len, fp)) != (size_t)-1 && i < LINE_NUM) {
        if(buffer[line_len-1] == '\n')
            buffer[line_len-1] = 0;

        unescape(buffer);

        logo[i+2] = mem + i*LINE_LEN;
        strncpy(logo[i+2], buffer, LINE_LEN);

        ++i;
    }

    // cleaning up
    fclose(fp);
    free(buffer);

    // set up the logo metadata;
    logo[0] = "custom"; // logo ID
    logo[2] = mem;      // the first characters of mem are used for the whitespace
    size_t j;
    for(j = 0; j < strlen_real(logo[3]); ++j) {   // filling in the whitespace
        mem[j] = ' ';
    }
    mem[j+1] = 0;
    
    // the array of lines is NULL-terminated;
    logo[i+2] = NULL;

    // finally, the logo can be saved
    config.logo = logo;

    return 0;
}

// add a module containing id to array
void add_module(struct Module *array, char *id) {
    struct Module *new = malloc(sizeof(struct Module));
    struct Module *last = array;

    for(struct Module *current = array; current->next; current = current->next)
        last = current->next;
    
    last->next = new;

    new->id = malloc(strlen(id)+1);
    strcpy(new->id, id);

    new->label = NULL;
    new->func = NULL;

    new->next = NULL;
}

// free every module in array
void destroy_array(struct Module *array) {
    struct Module *current = array;
    struct Module *next;

    while(current) {
        next = current->next;
        free(current->id);
        free(current);
        current = next;
    }
}

// print a certain line of the logo
void get_logo_line(char *dest, unsigned *line) {
    if(!config.logo || !dest || *line < 2)
        return;
        
    if(config.logo[(*line)+1]) {
        ++(*line);
        strcat(dest, config.logo[*line]);
    }
    else
        strcat(dest, config.logo[2]);
}

// print no more than maxlen visible characters of line
void print_line(char *line, const size_t maxlen) {
    if(bold)
        fputs("\033[1m", stdout);

    bool escaping = false;
    int unicode = 0;

    for(size_t i = 0, len = 0; len < maxlen && i < strlen(line); ++i) {
            putc(line[i], stdout);

            if(line[i] == '\n')
                break;
            else if(line[i] == '\033')
                escaping = true;
            else if(line[i] & 0x80) {       // is the 1st bit 1?
                if(line[i] & 0x40) {        // is the 2nd bit 1?
                    if(line[i] & 0x20) {    // is the 3rd bit 1?
                        if(line[i] & 0x10)  // is the 4th bit 1?
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

                /* m is found     and escaping     => escaping = 0
                 * m is found     and not escaping => escaping = 0
                 * m is not found and escaping     => escaping = 1
                 * m is not found and not escaping => escaping = 0
                 */
                escaping = (line[i] != 'm') && escaping;
            }
        }

    fputs("\033[0m\n", stdout);
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
    
    size_t len = strlen(ptr);
    if(len+1 > maxlen) {
        memcpy(dest, ptr, maxlen);
        dest[maxlen-1] = 0;
    } else
        memcpy(dest, ptr, len+1);


    *end = '"';

    return 0;
}

// a return code of 0 means that the option was parsed successfully
int parse_config_int(const char *source, const char *field, int *dest, const unsigned max) {
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

    if((unsigned)num > max)
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
    *dest = strcmp(ptr, "false");
    *end = '"';

    return 0;
}

// parse the provided config file
void parse_config(const char *file, struct Module *modules, char *mem, bool *default_bold, char *default_color, char *default_logo) {
    FILE *fp = fopen(file, "r");

    if(!fp)
        return;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    
    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    // remove comments
    char *ptr = conf, *ptr2;
    while((ptr = strchr(ptr, ';'))) {
        if(ptr != conf) {   // don't want to check ptr[-1] on the first character of the buffer ;)
            if(ptr[-1] == '\\') {
                ++ptr;
                continue;
            }
        }

        ptr2 = strchr(ptr, '\n');
        
        if(!ptr2) {
            *ptr = 0;
            break;
        }

        memmove(ptr, ptr2+1, strlen(ptr2));
    }
    ptr = conf;
    while((ptr = strchr(ptr, '#'))) {
        if(ptr != conf) {   // same as above
            if(ptr[-1] == '\\') {
                ++ptr;
                continue;
            }
        }

        ptr2 = strchr(ptr, '\n');

        if(!ptr2) {
            *ptr = 0;
            break;
        }

        memmove(ptr, ptr2+1, strlen(ptr2));
    }

    unescape(conf);

    // GENERAL OPTIONS

    // ascii art
    char path[96] = "";
    parse_config_str(conf, "ascii_art", path, sizeof(path));
    if(path[0])
        file_to_logo(path, mem + 2048);
    
    // logo
    char logo[32] = "";
    parse_config_str(conf, "logo", logo, sizeof(logo));
    if(logo[0]) {
        for(size_t i = 0; i < sizeof(logos)/sizeof(logos[0]); ++i)
            if(!strcmp(logos[i][0], logo)) {
                config.logo = logos[i];
                strcpy(default_logo, logos[i][0]);
                strcpy(config.color, logos[i][1]);
            }
    }

    // color
    char color[16] = "";
    parse_config_str(conf, "default_color", color, sizeof(color));
    if(color[0]) {
        char *colors[][2] = {
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
    parse_config_str(conf, "separator_character", config.separator, sizeof(config.separator));

    // BOOLEAN OPTIONS (check utils.h)
    char *booleanOptions[] = {
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
    };

    bool buffer;
    for(size_t i = 0; i < sizeof(booleanOptions)/sizeof(booleanOptions[0]); ++i) {
        if(!parse_config_bool(conf, booleanOptions[i], &buffer)) {
            if(buffer)
                config.options |= ((uint64_t)1 << i);
            else
                config.options &= ~((uint64_t)1 << i);
        }
    }
    *default_bold = bold;

    // OTHER MODULE-RELATED OPTIONS

    parse_config_int(conf, "gpu_index", &config.gpu_index, 3);

    parse_config_str(conf, "date_format", config.date_format, sizeof(config.date_format));

    parse_config_int(conf, "col_block_len", &config.col_block_len, 16);

    // LABELS

    char *prefixes[][2] = {
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
        {config.colors_prefix, "colors_prefix"},
        {config.light_colors_prefix, "colors_light_prefix"},
    };

    for(size_t i = 0; i < sizeof(prefixes)/sizeof(prefixes[0]); ++i)
        parse_config_str(conf, prefixes[i][1], prefixes[i][0], 64);

    // MODULES
    
    ptr = strstr(conf, "modules");
    if(!ptr) {
        free(conf);
        return;
    }

    ptr2 = strchr(ptr, '{');
    if(!ptr2) {
        free(conf);
        return;
    }

    char *end = strchr(ptr2, '}');
    if(!end) {
        free(conf);
        return;
    }
    *end = 0;

    while((ptr = strchr(ptr2, '"'))) {
        ++ptr;

        ptr2 = strchr(ptr, '"');
        if(!ptr2) {
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

// check every '\' in str and unescape "\\" "\n" "\e" "\033"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        switch(str[1]) {
            case 'e':
                memmove(str, str+1, strlen(str));
                *str = '\033';
                break;
            case '0':
                if(str[2] == '3' && str[3] == '3') {
                    memmove(str, str+3, strlen(str+2));
                    *str = '\033';
                }
                break;
            case 'n':
                memmove(str, str+1, strlen(str));
                *str = '\n';
                break;
            default:    // takes care of "\\", "\;" and "\#"
                memmove(str, str+1, strlen(str));
        }
        ++str;
    }
}

// get the printed length of a string (not how big it is in memory)
size_t strlen_real(const char *str) {
    if(str == NULL)
        return 0;

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

