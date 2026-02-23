#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#else
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#ifdef __ANDROID__
#include <ctype.h>
#endif // __ANDROID__
#endif // __APPLE__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

int battery(char *dest) {
    char capacity[5] = "";
    char status[20] = "";

#ifdef __ANDROID__ // relies on termux api
    char buf[DEST_SIZE];
    char *args[] = {"termux-battery-status", NULL};
    execCmd(buf, DEST_SIZE, args);

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
    CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourceInfo);

    if(CFArrayGetCount(powerSourcesList) == 0)
        return ERR_NO_INFO;

    CFDictionaryRef powerSource = CFArrayGetValueAtIndex(powerSourcesList, 0);

    long current_capacity = 0;
    long max_capacity = 0;

    CFNumberRef currentRef = CFDictionaryGetValue(powerSource, CFSTR(kIOPSCurrentCapacityKey));
    CFNumberRef maxRef = CFDictionaryGetValue(powerSource, CFSTR(kIOPSMaxCapacityKey));

    if (currentRef && maxRef) {
        CFNumberGetValue(currentRef, kCFNumberLongType, &current_capacity);
        CFNumberGetValue(maxRef, kCFNumberLongType, &max_capacity);

        snprintf(capacity, 5, "%ld", (current_capacity * 100) / max_capacity);
    }
    else
        return ERR_GENERIC;

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

    if(capacity[0] != 0 && status[0] != 0) {
        if((_bat_status))
            snprintf(dest, DEST_SIZE, "%s%% (%s)", capacity, status);
        else
            snprintf(dest, DEST_SIZE, "%s%%", capacity);
    } else if(capacity[0] != 0)
        snprintf(dest, DEST_SIZE, "%s%%", capacity);
    else if(status[0] != 0 && (_bat_status))
        safeStrncpy(dest, status, DEST_SIZE);
    else
        return ERR_NO_INFO;

    return RET_OK;
}
