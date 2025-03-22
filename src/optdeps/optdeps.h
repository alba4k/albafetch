/*
 * This whole idea might be really stupid.
 * It provides custom implementations of functions if
 * the related library (that I would prefer using) is
 * not found at compile time. It probably makes no sense
 * to do this (as I could probably just always use my own
 * implementation), but who cares
 */

#pragma once

#include <stdbool.h>

// can use libc or alternatives
bool binaryInPath(const char *str);

void getGpus(char **gpus);
