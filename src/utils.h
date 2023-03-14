#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

void print_line(char **logo, unsigned *line);

void unescape(char *str);

int max(const int *nums, unsigned const int lenght);

// needed for 
struct MemoryStruct {
  char *memory;
  size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif // UTILS_H