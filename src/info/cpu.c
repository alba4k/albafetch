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
    size_t read = 0;

#ifdef __APPLE__
    size_t BUF_SIZE = DEST_SIZE;
    char buf[BUF_SIZE];
    buf[0] = 0;
    sysctlbyname("machdep.cpu.brand_string", buf, &BUF_SIZE, NULL, 0);

    if(buf[0] == 0)
        return ERR_NO_INFO;

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
        return ERR_NO_FILE;

    char *buf = malloc(0x10000);
    if(buf == NULL)
        return ERR_OOM;

    read = fread(buf, sizeof(*buf), 0x10000, fp);
    fclose(fp);
    if(read > 0)
        buf[read - 1] = 0;
    else {
        free(buf);
        return ERR_NO_INFO;
    }

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
        return ERR_PARSING;
    }

    cpu_info += 13;

    end = strstr(cpu_info, " @");
    if(end)
        *end = 0;
    else {
        end = strchr(cpu_info, '\n');
        if(end == NULL) {
            free(buf);
            return ERR_PARSING + 0x10;
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

                snprintf(freq, 24, " @ %.2g GHz", atof(frequency) / 1e3);
            }
        }
    }
#endif

    // cleaning the string from various garbage
    if((end = strstr(cpu_info, "(R)")))
        memmove(end, end + 3, strlen(end + 3) + 1);
    if((end = strstr(cpu_info, "(TM)")))
        memmove(end, end + 4, strlen(end + 4) + 1);
    if((end = strstr(cpu_info, " CPU")))
        memmove(end, end + 4, strlen(end + 4) + 1);
    if((end = strstr(cpu_info, "th Gen ")))
        memmove(end - 2, end + 7, strlen(end + 7) + 1);
    if((end = strstr(cpu_info, " with Radeon Graphics")))
        *end = 0;
    if((end = strstr(cpu_info, "-Core Processor"))) {
        if(end >= cpu_info + 5) {
            end -= 5;
            end = strchr(end, ' ');
            if(end != NULL)
                *end = 0;
        }
    }

    if((_cpu_brand) == 0) {
        if((end = strstr(cpu_info, "Intel Core ")))
            memmove(end, end + 11, strlen(end + 1));
        else if((end = strstr(cpu_info, "Apple ")))
            memmove(end, end + 6, strlen(end + 6) + 1);
        else if((end = strstr(cpu_info, "AMD ")))
            memmove(end, end + 4, strlen(end + 1));
    }

    safeStrncpy(dest, cpu_info, DEST_SIZE);
#ifndef __APPLE__
    free(buf);
#endif

    if(freq[0])
        strncat(dest, freq, DEST_SIZE - 1 - strlen(dest));

    // final cleanup ("Intel Core i5         650" lol)
    while((end = strstr(dest, "  ")))
        memmove(end, end + 1, strlen(end));

    if(count && _cpu_count) {
        char core_count[16];
        snprintf(core_count, sizeof(core_count), " (%d)", count);
        strncat(dest, core_count, DEST_SIZE - 1 - strlen(dest));
    }
    if(_cpu_temp) {
        FILE *fp = fopen("/sys/class/thermal/thermal_zone2/temp", "r");
        if(fp == NULL)
            return ERR_NO_FILE;

        char buf[16] = "";
        read = fread(buf, sizeof(*buf), sizeof(buf), fp);
        fclose(fp);

        if(read <= 0)
            return RET_OK;
        
        buf[read - 1] = 0;

        if(buf[0] != 0) {
            int temp = atoi(buf)/1000;
            snprintf(buf, sizeof(buf), " [%dC]", temp);
            strncat(dest, buf, DEST_SIZE - strlen(dest));
        }
    }

    return RET_OK;
}
