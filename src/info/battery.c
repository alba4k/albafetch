#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>
#else
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __ANDROID__
#include "../utils/wrappers.h"
#include <ctype.h>
#endif // __ANDROID__
#endif // __APPLE__

#include "../config/config.h"
#include "info.h"

SInfo *battery(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    char capacity[5] = "";
    char status[20] = "";

#ifdef __ANDROID__ // relies on termux api
    char buf[64];
    char *args[] = {"termux-battery-status", NULL};
    execCmd(buf, sizeof(buf), args);

    char *ptr = strstr(buf, "percentage");
    char *ptr2 = strstr(buf, "status");
    char *end;

    if(ptr != NULL) {
        ptr += 13;
        end = strchr(ptr, ',');
        if(end != NULL) {
            *end = 0;
            safeStrncpy(capacity, ptr, 5);
        }
    }

    if(ptr2 != NULL) {
        ptr2 += 10;
        end = strstr(ptr2, "\",");
        ptr = ptr2 + 1;
        while(*ptr) {
            *ptr = tolower(*ptr);

            ++ptr;
        }

        if(end != NULL) {
            *end = 0;
            safeStrncpy(status, ptr2, 20);
        }
    }
#else
#ifdef __APPLE__
    CFTypeRef powerSourceInfo = IOPSCopyPowerSourcesInfo();
    if(powerSourceInfo == NULL) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourceInfo);
    if(powerSourcesList == NULL) {
        CFRelease(powerSourceInfo);
        res->retval = ERR_NO_INFO;
        return res;
    }

    if(CFArrayGetCount(powerSourcesList) == 0) {
        CFRelease(powerSourcesList);
        CFRelease(powerSourceInfo);
        res->retval = ERR_NO_INFO;
        return res;
    }

    CFDictionaryRef powerSource = CFArrayGetValueAtIndex(powerSourcesList, 0);

    long current_capacity = 0;
    long max_capacity = 0;

    CFNumberRef currentRef = CFDictionaryGetValue(powerSource, CFSTR(kIOPSCurrentCapacityKey));
    CFNumberRef maxRef = CFDictionaryGetValue(powerSource, CFSTR(kIOPSMaxCapacityKey));

    if(currentRef && maxRef) {
        CFNumberGetValue(currentRef, kCFNumberLongType, &current_capacity);
        CFNumberGetValue(maxRef, kCFNumberLongType, &max_capacity);

        if(max_capacity > 0)
            snprintf(capacity, 5, "%ld", (current_capacity * 100) / max_capacity);
        else {
            CFRelease(powerSourcesList);
            CFRelease(powerSourceInfo);
            res->retval = ERR_NO_INFO;
            return res;
        }
    }
    else {
        CFRelease(powerSourcesList);
        CFRelease(powerSourceInfo);
        res->retval = ERR_NO_INFO;
        return res;
    }

    CFRelease(powerSourcesList);
    CFRelease(powerSourceInfo);
#else
    FILE *fp = NULL;

    if((fp = fopen("/sys/class/power_supply/BAT0/capacity", "r"))) {
        size_t read = fread(capacity, sizeof(*capacity), 5, fp);
        fclose(fp);

        if(read > 0)
            capacity[read - 1] = 0;
        else
            status[0] = 0;
    }
    if((fp = fopen("/sys/class/power_supply/BAT0/status", "r"))) {
        size_t read = fread(status, sizeof(*status), 12, fp);
        fclose(fp);

        if(read > 0)
            status[read - 1] = 0;
        else
            status[0] = 0;
    }
#endif // __APPLE__
#endif // __ANDROID__

    if(capacity[0] != 0 && status[0] != 0 && config.bat_status) {
        const size_t len = strlen(capacity) + strlen(status) + 5;
        res->string = malloc(len);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, len, "%s%% (%s)", capacity, status);
    }
    else if(capacity[0] != 0) {
        const size_t len = strlen(capacity) + 2;
        res->string = malloc(len);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
        snprintf(res->string, len, "%s%%", capacity);
    }
    else if(status[0] != 0 && config.bat_status) {
        res->string = strdup(status);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            return res;
        }
    }
    else
        res->retval = ERR_NO_INFO;

    return res;
}
