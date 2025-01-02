#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef __ANDROID__
#include <ctype.h>
#endif // __ANDROID__

#include "info.h"
#include "../config/config.h"
#include "../utils/wrappers.h"

// get the battery percentage and status (Linux only!)
int battery(char *dest) {
    char capacity[5] = "";
    char status[20] = "";
    
    #ifdef __ANDROID__  // relies on termux api
        char buf[256];
        char *args[] = {"termux-battery-status", NULL};
        exec_cmd(buf, 256, args);

        char *ptr = strstr(buf, "percentage");
        char *ptr2 = strstr(buf, "status");
        char *end;

        if(ptr != NULL) {
            ptr += 13;
            end = strchr(ptr, ',');
            if(end != NULL) {
                *end = 0;
                safe_strncpy(capacity, ptr, 5);
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
                safe_strncpy(status, ptr2, 20);
            }
        }
    #else
        FILE *fp = NULL;

        if((fp = fopen("/sys/class/power_supply/BAT0/capacity", "r"))) {
            capacity[fread(capacity, 1, 5, fp) - 1] = 0;

            fclose(fp);
        }
        if((fp = fopen("/sys/class/power_supply/BAT0/status", "r"))) {
            status[fread(status, 1, 20, fp) - 1] = 0;

            fclose(fp);
        }
    #endif // __ANDROID__

    if(capacity[0] != 0 && status[0] != 0) {
        if((_bat_status))
            snprintf(dest, 256, "%s%% (%s)", capacity, status);
        else
            snprintf(dest, 256, "%s%%", capacity);
    }
    else if(capacity[0] != 0)
        snprintf(dest, 256, "%s%%", capacity);
    else if(status[0] != 0 && (_bat_status))
        safe_strncpy(dest, status, 256);
    else
        return 1;

    return 0;
}   
