#include <stdlib.h>
#include <sys/sysctl.h>


/* 
 * Header for BSD standard system-querying 
 * functions (also applicable to macos and
 * possibly linux).
 */

/*
 * This is a wrapper function over the fairly 
 * cryptic BSD `sysctl` function.
 *
 * Note that `out` is meant to be casted by the
 * user as it can return various types depending on the query.
 * If the type is _slightly_ incorrect, the function will fail.
 *
 * The function returns 0 if the query was successful,
 * or -1 if the query failed.
 *
 * To find the expected values for `domain` and `field`,
 * refer to the `sysctl` man pages.
 *
 * ex. For querying total memory size.
 *
 * ```c
 * uin64_t size;
 * size_t len = sizeof(uin64_t);
 * sysctl_wrap(&size, &len, 
 *             CTL_HW,      // Query the hardware domain.
 *             HW_MEMSIZE); // Get memory size from the hardware domain.
 * ```
 */
int sysctl_wrap(void *out, size_t outsize, 
                int domain, int field);

