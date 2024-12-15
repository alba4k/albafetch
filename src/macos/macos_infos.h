#pragma once

#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>

#include <mach/mach.h>
#include <stdint.h>
#include <AvailabilityMacros.h>

#include "bsdwrap.h"

typedef uint64_t bytes_t;
typedef uint64_t pages_t;

char *get_gpu_string(void);

/*
 * Gets the used memory int
 */
bytes_t used_mem_size(void);

bytes_t system_mem_size(void);
