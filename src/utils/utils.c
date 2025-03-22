#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "return.h"
#include "wrappers.h"
#include "../config/config.h"
#include "../config/parsing.h"

// copy an ascii art from file to mem
void *fileToLogo(char *file) {
    FILE *fp = fopen(file, "r");
    if(fp == NULL)
        return NULL;

/* 
     * mem is assumed to be a 10 KiB buffer, aka 10240 B.
     * this will be filled in with up to 64 lines,
     * each of which can be up to 160 bytes long.
     * (LINE_LEN * LINE_COUNT) should equal this size.
     * Check out main.c, line 132
    */
#define LINE_LEN   256
#define LINE_COUNT 40

    // where the final logo is saved
    static char *logo[LINE_COUNT + 1];
    char *mem = NULL;

    char *buffer = NULL;
    size_t len = 0;
    size_t line_len;
    ptrdiff_t i = 0;

    // setting the correct color (or eventually the first line)

    line_len = getline(&buffer, &len, fp); // save the first line to buffer
    if(line_len == (size_t)-1)             // getline returns -1 in case of error
        return NULL;

    if(buffer[line_len - 1] == '\n')
        buffer[line_len - 1] = 0;

    config.color[0] = 0;

    const char *colors[][2] = {
        {"black", "\033[30m"},  {"red", "\033[31m"},  {"green", "\033[32m"}, {"yellow", "\033[33m"}, {"blue", "\033[34m"},
        {"purple", "\033[35m"}, {"cyan", "\033[36m"}, {"gray", "\033[90m"},  {"white", "\033[37m"},
    };

    for(int j = 0; j < 9; ++j)
        if(strcmp(buffer, *colors[j]) == 0)
            strcpy(config.color, colors[j][1]);

    mem = malloc(1024);
    if(mem == NULL)
        return NULL;
    memset(mem, 0, 1024);

    if(config.color[0] == 0) {
        unescape(buffer);

        logo[i + 2] = mem + (i * LINE_LEN);
        safeStrncpy(logo[i + 2], buffer, LINE_LEN);

        ++i;
    }

    // for every remaining line of the logo...
    while((line_len = getline(&buffer, &len, fp)) != (size_t)-1 && i < LINE_COUNT) {
        if(buffer[line_len - 1] == '\n')
            buffer[line_len - 1] = 0;

        unescape(buffer);

        logo[i + 2] = mem + (i * LINE_LEN);
        safeStrncpy(logo[i + 2], buffer, LINE_LEN);

        ++i;
    }

    // cleaning up
    fclose(fp);
    free(buffer);

    // set up the logo metadata;
    logo[0] = "custom"; // logo ID

    // the array of lines is NULL-terminated;
    logo[i + 2] = NULL;

    // finally, the logo can be saved
    config.logo = logo;

    return mem;
}

// add a module containing id to array
void addModule(struct SModule *array, char *id) {
    struct SModule *new = malloc(sizeof(struct SModule));
    if(new == NULL)
        return;
    struct SModule *last = array;

    for(struct SModule *current = array; current->next; current = current->next)
        last = current->next;

    last->next = new;

    new->id = malloc(strlen(id) + 1);
    if(new->id == NULL)
        return;
    strcpy(new->id, id);

    new->label = NULL;
    new->func = NULL;

    new->next = NULL;
}

// free every module in array
void destroyArray(struct SModule *array) {
    struct SModule *current = array;
    struct SModule *next;

    while(current) {
        next = current->next;
        free(current->id);
        free(current);
        current = next;
    }
}

// print a certain line of the logo
void getLogoLine(char *dest, unsigned *line) {
    if(config.logo == NULL || dest == NULL || *line < 1)
        return;

    if(config.logo[(*line) + 1]) {
        ++(*line);
        strcat(dest, config.logo[*line]);
    } else {
        for(size_t i = 0; i < realStrlen(config.logo[2]); ++i)
            strcat(dest, " ");
    }
}

// print no more than maxlen visible characters of line
void printLine(char *line, const size_t maxlen) {
    if(_bold)
        fputs("\033[1m", stdout);
    fputs(config.color, stdout);

    bool escaping = false;
    int unicode = 0;

    for(size_t i = 0, len = 0; len < maxlen && i < strlen(line); ++i) {
        putc(line[i], stdout);

        if(line[i] == '\n')
            break;
        else if(line[i] == '\033')
            escaping = true;
        else if(line[i] & 0x80) {      // is the 1st bit 1?
            if(line[i] & 0x40) {       // is the 2nd bit 1?
                if(line[i] & 0x20) {   // is the 3rd bit 1?
                    if(line[i] & 0x10) // is the 4th bit 1?
                        unicode = 3;   // first of 4 unicode bytes (0b11110xxx)
                    else
                        unicode = 2; // first of 3 unicode bytes (0b1110xxxx)
                } else
                    unicode = 1;      // first of 2 unicode bytes (0b110xxxxx)
            } else if(unicode-- == 1) // unicode continuation byte (0b10xxxxxx)
                ++len;
        } else {
            // look mom, I just wanted to try to write some branchless code

            // this line is a bit weird
            // ++len <=> escaping == 0
            len += (size_t)1 - escaping;

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
