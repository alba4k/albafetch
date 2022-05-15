#pragma once

#ifndef MACOS_INFOS_H
#define MACOS_INFOS_H

#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>

#include <mach/mach.h>

#include "bsdwrap.h"

typedef uint64_t bytes_t;
typedef uint64_t pages_t;

const char *get_gpu_string();

/*
 * Gets the used memory int
 */
bytes_t used_mem_size();

bytes_t system_mem_size();

#endif //MACOS_INFOS_H 
