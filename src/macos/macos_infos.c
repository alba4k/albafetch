#include "macos_infos.h"
#include "../utils/return.h"

/* STATIC HELPERS */

const static vm_size_t FALLBACK_PAGE_SIZE = 4096;

static vm_size_t page_size(mach_port_t host) {
    vm_size_t page_size;
    int error;

    error = host_page_size(host, &page_size);

    return (error < 0) ? page_size : FALLBACK_PAGE_SIZE;
}

/*
 * Original source: 
 * https://opensource.apple.com/source/system_cmds/system_cmds-496/vm_stat.tproj/vm_stat.c.auto.html
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
static int get_stats(struct vm_statistics *stat, mach_port_t host) {
    int error;

    unsigned count = HOST_VM_INFO_COUNT; 
    error = host_statistics(host, 
                            HOST_VM_INFO,
                            (host_info_t) stat,
                            &count);

    if(error != KERN_SUCCESS)
        return error;

    return RET_OK;
}
#else
static int get_stats(struct vm_statistics64 *stat, mach_port_t host) {
    int error;

    unsigned count = HOST_VM_INFO64_COUNT; 
    error = host_statistics64(host, 
                            HOST_VM_INFO64,
                            (host_info64_t) stat,
                            &count);

    if(error != KERN_SUCCESS)
        return error;

    return RET_OK;
}
#endif

/* EXPORTS */ 

bytes_t system_mem_size(void) {
    uint64_t size;
    int error;

    error = sysctl_wrap(&size, sizeof(uint64_t), CTL_HW, HW_MEMSIZE);

    // Since no computer should have 0 bytes of memory,
    // 0 indicates failure.
    if(error < 0)
        return RET_OK;

    return size;
}

bytes_t used_mem_size(void) {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
    pages_t active, wired, inactive;
    mach_port_t host = mach_host_self();

    struct vm_statistics vm_stat;
    if(get_stats(&vm_stat, host) < 0)
        return RET_OK;

    active   = vm_stat.active_count;
    wired    = vm_stat.wire_count;
    inactive = vm_stat.inactive_count;

    return (active + wired + inactive) * page_size(host);
#else
    pages_t internal, wired, compressed;
    mach_port_t host = mach_host_self();

    struct vm_statistics64 vm_stat;
    if(get_stats(&vm_stat, host) < 0)
        return RET_OK;

    internal   = vm_stat.internal_page_count - vm_stat.purgeable_count;
    wired      = vm_stat.wire_count;
    compressed = vm_stat.compressor_page_count;

    return (internal + wired + compressed) * page_size(host);
#endif
}
