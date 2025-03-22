#pragma once

#ifdef __APPLE__

#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>

#include <mach/mach.h>
#include <stdint.h>
#include <AvailabilityMacros.h>

#include "bsdwrap.h"

typedef uint64_t bytes_t;
typedef uint64_t pages_t;

char *getGpuString(void);

/*
 * Gets the used memory int
 */
bytes_t usedMemSize(void);

bytes_t systemMemSize(void);

#endif // __APPLE__
