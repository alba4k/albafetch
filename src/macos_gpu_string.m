#include "macos_infos.h"

#import <Foundation/Foundation.h>

char *get_gpu_string() {
    CFMutableDictionaryRef dict = IOServiceMatching("IOPCIDevice");
    io_iterator_t iter;
    int success;
    const char *result;

    mach_port_t port = kIOMainPortDefault; // maybe kIOMasterPortDefault but it's apparently deprecated since macOS Monterey
    success = IOServiceGetMatchingServices(port, dict, &iter);
    if(success != kIOReturnSuccess)
        return NULL;

    io_registry_entry_t entry;

    while((entry = IOIteratorNext(iter))) {
        CFMutableDictionaryRef services;
        success = IORegistryEntryCreateCFProperties(entry, &services, kCFAllocatorDefault, kNilOptions);
        if(success != kIOReturnSuccess) {
            IOObjectRelease(entry);
            continue;
        }

        const void *gpu_model = CFDictionaryGetValue(services, @"model");
        if(gpu_model != nil) {
            if(CFGetTypeID(gpu_model) == CFDataGetTypeID()) {
                NSString *modelName = [[NSString alloc] initWithData:
                                        (NSData *)gpu_model encoding:NSASCIIStringEncoding];
                
                result = [modelName cStringUsingEncoding:NSUTF8StringEncoding];
            }
        } else {
            return NULL;
        }

        CFRelease(services);
        IOObjectRelease(entry);
    }

    IOObjectRelease(iter);

    return (char*)result;
}
