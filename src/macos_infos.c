#include "macos_infos.h"

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
static int get_stats(struct vm_statistics64 *stat, mach_port_t host) {
    int error;

    unsigned count = HOST_VM_INFO64_COUNT; 
    error = host_statistics64(host, 
                            HOST_VM_INFO64,
                            (host_info64_t) stat,
                            &count);

    if(error != KERN_SUCCESS)
        return error;

    return 0;
}

/* EXPORTS */ 

bytes_t system_mem_size(void) {
    uint64_t size;
    int error;

    error = sysctl_wrap(&size, sizeof(uint64_t), CTL_HW, HW_MEMSIZE);

    // Since no computer should have 0 bytes of memory,
    // 0 indicates failure.
    if(error < 0)
        return 0;

    return size;
}

bytes_t used_mem_size(void) {
    pages_t internal, wired, compressed;
    mach_port_t host = mach_host_self();

    struct vm_statistics64 vm_stat;
    if(get_stats(&vm_stat, host) < 0)
        return 0;

    internal   = vm_stat.internal_page_count - vm_stat.purgeable_count;
    wired      = vm_stat.wire_count;
    compressed = vm_stat.compressor_page_count;

    return (internal + wired + compressed) * page_size(host);
}
