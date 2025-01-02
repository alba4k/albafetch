#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif // __APPLE__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the cpu name and frequency
int cpu(char *dest) {
    char *cpu_info;
    char *end;
    int count = 0;
    char freq[24] = "";

    #ifdef __APPLE__
        size_t BUF_SIZE = 256;
        char buf[BUF_SIZE];
        buf[0] = 0;
        sysctlbyname("machdep.cpu.brand_string", buf, &BUF_SIZE, NULL, 0);

        if(buf[0] == 0)
            return 1;

        if((_cpu_freq) == 0) {
            if((end = strstr(buf, " @")))
                *end = 0;
            else if((end = strchr(buf, '@')))
                *end = 0;
        }

        cpu_info = buf;
    #else
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(fp == NULL)
        return 1;

    char *buf = malloc(0x10001);
    buf[fread(buf, 1, 0x10000, fp)] = 0;
    fclose(fp);

    cpu_info = buf;
    if(_cpu_count) {
        end = cpu_info;
        while((end = strstr(end, "processor"))) {
            ++count;
            ++end;
        }
    }

    cpu_info = strstr(cpu_info, "model name");
    if(cpu_info == NULL) {
        free(buf);
        return 1;
    }

    cpu_info += 13;

    end = strstr(cpu_info, " @");
    if(end)
        *end = 0;
    else {
        end = strchr(cpu_info, '\n');
        if(end == NULL) {
            free(buf);
            return 1;
        }
            
        *end = 0;
    }

    /* I might eventually add an option to get the "default" clock speed
     * by parsing one or more of the following files:
     * - /sys/devices/system/cpu/cpu0/cpufreq/cpupower_max_freq
     * - /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
     * - /sys/devices/system/cpu/cpu0/cpufreq/bios_limit
     * - /sys/devices/system/cpu/cpu0/cpufreq/base_frequency
     */
    // Printing the clock frequency the first thread is currently running at
    ++end;
    char *frequency = strstr(end, "cpu MHz");
    if(frequency && _cpu_freq) {
        frequency = strchr(frequency, ':');
        if(frequency) {
            frequency += 2;

            end = strchr(frequency, '\n');
            if(end) {
                *end = 0;

                snprintf(freq, 24, " @ %g GHz", (float)(atoi(frequency)/100) / 10);
            }
        }
    }
    #endif

    // cleaning the string from various garbage
    if((end = strstr(cpu_info, "(R)")))
        memmove(end, end+3, strlen(end+3)+1);
    if((end = strstr(cpu_info, "(TM)")))
        memmove(end, end+4, strlen(end+4)+1);
    if((end = strstr(cpu_info, " CPU")))
        memmove(end, end+4, strlen(end+4)+1);
    if((end = strstr(cpu_info, "th Gen ")))
        memmove(end-2, end+7, strlen(end+7)+1);
    if((end = strstr(cpu_info, " with Radeon Graphics")))
        *end = 0;
    if((end = strstr(cpu_info, "-Core Processor"))) {
        if(end >= cpu_info+5) {
            end -= 5;
            end = strchr(end, ' ');
            if(end != NULL)
                *end = 0;
        }
    }

    if((_cpu_brand) == 0) {
        if((end = strstr(cpu_info, "Intel Core ")))
            memmove(end, end+11, strlen(end+1));
        else if((end = strstr(cpu_info, "Apple ")))
            memmove(end, end+6, strlen(end+6)+1);
        else if((end = strstr(cpu_info, "AMD ")))
            memmove(end, end+4, strlen(end+1));
    }

    safe_strncpy(dest, cpu_info, 256);
    #ifdef __linux__
        free(buf);
    #endif

    if(freq[0])
        strncat(dest, freq, 255-strlen(dest));

    if(count && _cpu_count) {
        char core_count[16];
        snprintf(core_count, 16, " (%d) ", count);
        strncat(dest, core_count, 255-strlen(dest));
    }
    // final cleanup ("Intel Core i5         650" lol)
    while((end = strstr(dest, "  ")))
        memmove(end, end+1, strlen(end));

    return 0;
}
