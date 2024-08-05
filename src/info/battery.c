#include "info.h"
#include "../utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef __ANDROID__
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#endif // __ANDROID__


// get the battery percentage and status (Linux only!)
int battery(char *dest) {
    char capacity[5] = "";
    char status[20] = "";
    
    #ifdef __ANDROID__  // relies on termux api
        int pipes[2];
        char buf[256];

        if(pipe(pipes) != 0)
            return 1;
        if(fork() == 0) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("termux-battery-status", "termux-battery-status", NULL); 
        }

        wait(0);
        close(pipes[1]);
        buf[read(pipes[0], buf, 256) - 1] = 0;
        close(pipes[0]);

        char *ptr = strstr(buf, "percentage");
        char *ptr2 = strstr(buf, "status");
        char *end;

        if(ptr != NULL) {
            ptr += 13;
            end = strchr(ptr, ',');
            if(end != NULL) {
                *end = 0;
                strncpy(capacity, ptr, 5);
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
                strncpy(status, ptr2, 20);
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
