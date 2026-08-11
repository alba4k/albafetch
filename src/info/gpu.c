// TODO: refactor this shit

#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef __APPLE__
#include <sys/utsname.h>
#include "../macos/macos_infos.h"
#include "../utils/wrappers.h"
#else
#include <vulkan/vulkan.h>
#endif // __APPLE__

// get the gpu name(s)
int gpu(char *dest) {
    char *gpus[] = {NULL, NULL, NULL};

#ifdef __APPLE__
    struct utsname name;
    uname(&name);

    if(strcmp(name.machine, "x86_64") == 0)
        gpus[0] = getGpuString();                        // only works on x64
    if(gpus[0] == 0 || strcmp(name.machine, "x86_64")) { // fallback
        char buf[1024];
        char *args[] = {"/usr/sbin/system_profiler", "SPDisplaysDataType", NULL};
        execCmd(buf, 1024, args);

        gpus[0] = strstr(buf, "Chipset Model: ");
        if(gpus[0] == 0)
            return ERR_NO_INFO;
        gpus[0] += 15;
        char *end = strchr(gpus[0], '\n');
        if(end == NULL)
            return ERR_PARSING;
        *end = 0;
    }
#else
    VkInstance instance;
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
    };

    if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS)
        return ERR_UNSUPPORTED;

    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, NULL);

    if(count == 0) {
        vkDestroyInstance(instance, NULL);
        return ERR_NO_INFO;
    }

    VkPhysicalDevice devices[3];
    if(count > 3)
        count = 3;
    vkEnumeratePhysicalDevices(instance, &count, devices);

    int added = 0;
    for(uint32_t i = 0; i < count; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);

        if((config.gpu_index == -1 && props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
            (config.gpu_index == -2 && props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ||
            (config.gpu_index == 0 && (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) ||
            (config.gpu_index > 0 && (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) && (config.gpu_index == (int)i + 1))) {
            char *current = malloc(strlen(props.deviceName) + 1);
            strcpy(current, props.deviceName);
            gpus[added++] = current;
        }
    }
    vkDestroyInstance(instance, NULL);
#endif // __APPLE__

    if(gpus[0] == NULL)
        return ERR_NO_INFO;

    // this next part is just random cleanup
    // also, I'm using end as a random char* - BaD pRaCtIcE aNd CoNfUsInG - lol stfu
    //  yk it's decent and it works
    dest[0] = 0;
    for(unsigned i = 0; i < sizeof(gpus) / sizeof(gpus[0]) && gpus[i] != NULL; ++i) {
        char *end;
        while((end = strstr(gpus[i], "(R)")))
            memmove(end, end + 3, strlen(end + 3) + 1);
        
        if((_gpu_brand) == 0) {
            if(strstr(gpus[i], "Intel ") || strstr(gpus[i], "Apple "))
                memmove(gpus[i], gpus[i] + 6, strlen(gpus[i] + 6) + 1);
            else if(strstr(gpus[i], "AMD "))
                memmove(gpus[i], gpus[i] + 4, strlen(gpus[i] + 4) + 1);
        }

        end = strstr(gpus[i], " (");
        if(end) // when using vulkan it's often "BRAND(R) GPU NAME (DRIVER ARCHITECTURE)"
            *end = 0;

        if((end = strstr(gpus[i], " Integrated Graphics Controller")))
            *end = 0;
        if((end = strstr(gpus[i], "-Core Processor")) && end > gpus[i]+1)
            *(end-2) = 0;
        if((end = strstr(gpus[i], " Rev. ")))
            *end = 0;

        // (finally) writing the GPU(s) into dest
        if(i > 0)
            strncat(dest, ", ", DEST_SIZE - strlen(dest));
        strncat(dest, gpus[i], DEST_SIZE - 1 - strlen(dest));
    }

    #ifndef __APPLE__
        for(int i = 0; i < 3; i++)
            free(gpus[i]);
    #endif

    return RET_OK;
}
