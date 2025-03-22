#include "bsdwrap.h"
#include "../utils/return.h"

#ifdef __APPLE__

int sysctlWrap(void *out, size_t outsize, int domain, int field) {
    int mib[] = {domain, field};
    int error = sysctl(mib, 2, out, &outsize, NULL, 0);

    if(error < 0)
        return ERR_NO_INFO;

    return RET_OK;
}

#endif // __APPLE__
