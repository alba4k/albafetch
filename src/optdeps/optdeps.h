/*
 * This whole idea might be really stupid.
 * It provides custom implementations of functions if
 * the related library (that I would prefer using) is
 * not found at compile time. It probably makes no sense
 * to do this (as I could probably just always use my own
 * implementation), but who cares
 */

#pragma once

#ifndef OPTDEPS_H
#define OPTDEPS_H

#include <stdbool.h>

// can use libc or alternatives
bool binary_in_path(const char *str);

void get_gpus(char **gpus);

#endif // OPTDEPS_H
