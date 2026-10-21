#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void *fileToLogo(char *file);

void getLogoLine(char *dest, unsigned *line);

void printLine(char *line, const size_t maxlen);
