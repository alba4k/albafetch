#pragma once

#include <stddef.h>

int exec_cmd(char *buf, size_t len, char *const *argv);

size_t real_strlen(const char *str);

void safe_strncpy(char *dest, const char *src, size_t n);
