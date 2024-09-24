#include "macos_infos.h"

#import <Availability.h> // this lets us know when to use deprecated apis
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>

char *get_gpu_string(void) {
    CFMutableDictionaryRef dict = IOServiceMatching("IOPCIDevice");
    io_iterator_t iter;
    int success;
    const char *result = NULL;

    #if __OSX_AVAILABLE_STARTING(__MAC_12_0,__IPHONE_NA)

    mach_port_t port = kIOMainPortDefault;

    #else
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"

    mach_port_t port = kIOMasterPortDefault; // kIOMasterPortDefault has been deprecated since macOS Monterey

    #pragma clang diagnostic pop

    #endif

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
        }

        CFRelease(services);
        IOObjectRelease(entry);
    }

    IOObjectRelease(iter);

    return (char*)result;
}
