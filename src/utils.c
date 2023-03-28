#include <string.h>
#include <stdio.h>
#include "utils.h"

void print_line(char **logo, unsigned *line, size_t maxlen) {
    if(!logo)
        return;

    int escaping = 0;

    printf("\n\e[0m%s", config.bold ? "\e[1m" : "");

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

                escaping = (logo[*line][i] != 'm') && escaping;
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
    for(int i = 0; i < 4 && ((long)(maxlen - strlen(logo[2]) - 4) > 0); ++i)
        putc(' ', stdout);
    
    ++(*line);
}

// check every '\\' in str and unescape "\\\\" "\\n" "\\e"
void unescape(char *str) {
    while((str = strchr(str, '\\'))) {
        printf("%s\n\n%c\n\n", str, *str);
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

// TODO
// this will be removed once I write a decent logic for the single modules
int separator(char *dest){(void)dest;return 1;}
int spacing(char *dest){(void)dest;return 1;}
int title(char *dest){(void)dest;return 1;}
