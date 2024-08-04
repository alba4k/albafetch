#include "info.h"
#include "../utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// get the battery percentage and status (Linux only!)
int battery(char *dest) {
    char capacity[5] = "";
    char status[20] = "";
    
    FILE *fp = NULL;

    if((fp = fopen("/sys/class/power_supply/BAT0/capacity", "r"))) {
        capacity[fread(capacity, 1, 5, fp) - 1] = 0;

        fclose(fp);
    }
    if((fp = fopen("/sys/class/power_supply/BAT0/status", "r"))) {
        status[fread(status, 1, 20, fp) - 1] = 0;

        fclose(fp);
    }

    if(capacity[0] != 0 && status[0] != 0) {
        if((bat_status))
            snprintf(dest, 256, "%s%% (%s)", capacity, status);
        else
            snprintf(dest, 256, "%s%%", capacity);
    }
    else if(capacity[0] != 0)
        snprintf(dest, 256, "%s%%", capacity);
    else if(status[0] != 0 && (bat_status))
        strncpy(dest, status, 256);
    else
        return 1;

    return 0;
}   