#include "bsdwrap.h"

int sysctl_wrap(void *out, size_t outsize, 
                int domain, int field) {
    int mib[] = { domain, field };
    int error = sysctl(mib, 2, out, &outsize, NULL, 0);

    if (error < 0)
        return -1;

    return 0;
}