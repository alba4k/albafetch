#pragma once

#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>

bool is_in_string(const char *str, const char *place);

char *skip_whites(char *ptr);

char *skip_full(char *ptr);

#endif // PARSING_H
