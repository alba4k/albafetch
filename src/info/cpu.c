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
SInfo *cpu(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char *cpu_info;
    char *end;
    int count = 0;
    char freq[24] = "";
    size_t read = 0;

#ifdef __APPLE__
    size_t BUF_SIZE = 256;
    char buf[BUF_SIZE];
    buf[0] = 0;
    sysctlbyname("machdep.cpu.brand_string", buf, &BUF_SIZE, NULL, 0);

    if(buf[0] == 0) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    if(!_cpu_freq) {
        if((end = strstr(buf, " @")))
            *end = 0;
        else if((end = strchr(buf, '@')))
            *end = 0;
    }

    cpu_info = buf;
#else
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if(fp == NULL) {
        res->retval = ERR_NO_FILE;
        return res;
    }

    char *buf = malloc(0x10000);
    if(buf == NULL) {
        res->retval = ERR_OOM;
        return res;
    }

    read = fread(buf, sizeof(*buf), 0x10000, fp);
    fclose(fp);
    if(read > 0)
        buf[read - 1] = 0;
    else {
        free(buf);
        res->retval = ERR_NO_INFO;
        return res;
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
        res->retval = ERR_PARSING;
        return res;
    }

    cpu_info += 13;

    end = strstr(cpu_info, " @");
    if(end)
        *end = 0;
    else {
        end = strchr(cpu_info, '\n');
        if(end == NULL) {
            free(buf);
            res->retval = ERR_PARSING + 0x10;
            return res;
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
    if(frequency && _cpu_freq && (frequency = strchr(frequency, ':'))) {
        frequency += 2;

        end = strchr(frequency, '\n');
        if(end) {
            *end = 0;

            snprintf(freq, 24, " @ %.2g GHz", atof(frequency) / 1e3);
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

    if(!_cpu_brand) {
        if((end = strstr(cpu_info, "Intel ")))
            memmove(end, end + 6, strlen(end + 6) + 1);
        else if((end = strstr(cpu_info, "Apple ")))
            memmove(end, end + 6, strlen(end + 6) + 1);
        else if((end = strstr(cpu_info, "AMD ")))
            memmove(end, end + 4, strlen(end + 4) + 1);
    }

    res->string = strdup(cpu_info);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }

#ifndef __APPLE__
    free(buf);
#endif

    if(freq[0]) {
        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(freq) + 1);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, freq);
        }
    }

    // final cleanup ("Intel Core i5         650" lol)
    while((end = strstr(res->string, "  ")))
        memmove(end, end + 1, strlen(end));

    if(count && _cpu_count) {
        char core_count[16];
        snprintf(core_count, sizeof(core_count), " (%d)", count);

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(core_count) + 1);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, core_count);
        }
    }
    if(_cpu_temp) {
        FILE *fp = fopen("/sys/class/thermal/thermal_zone2/temp", "r");
        if(fp == NULL)
            return res;

        char temp[16] = "";
        read = fread(temp, sizeof(*temp), sizeof(temp), fp);
        fclose(fp);

        if(read <= 0)
            return res;

        temp[read - 1] = 0;

        if(temp[0] == 0)
            return res;

        snprintf(temp, sizeof(temp), " [%dC]", atoi(temp) / 1000);

        char *tmpptr = realloc(res->string, strlen(res->string) + strlen(temp) + 1);
        if(tmpptr) {
            res->string = tmpptr;
            strcat(res->string, temp);
        }
    }

    return res;
}
