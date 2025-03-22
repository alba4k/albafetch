#pragma once

#include <stddef.h>

int execCmd(char *buf, size_t len, char *const *argv);

size_t realStrlen(const char *str);

void safeStrncpy(char *dest, const char *src, size_t n);
