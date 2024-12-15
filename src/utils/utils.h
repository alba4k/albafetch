#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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

void unescape(char *str);

int exec_cmd(char *buf, size_t len, char *const *argv);

size_t strlen_real(const char *str);
