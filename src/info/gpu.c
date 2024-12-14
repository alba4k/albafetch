#include "info.h"
#include "../config/config.h"
#include "../utils/utils.h"

#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef __APPLE__
#include <sys/utsname.h>
#include "../macos/macos_infos.h"
#else
#ifndef __ANDROID__
#include "../optdeps/optdeps.h"
#endif // __ANDROID__
#endif // __APPLE__

// get the gpu name(s)
int gpu(char *dest) {
    char *gpus[] = {NULL, NULL, NULL};
    char *end;

    #ifdef __APPLE__
        struct utsname name;
        uname(&name);

        if(strcmp(name.machine, "x86_64") == 0)
            gpus[0] = get_gpu_string();  // only works on x64
        if(gpus[0] == 0 || strcmp(name.machine, "x86_64")) {     // fallback
            char buf[1024];
            char *args[] = {"/usr/sbin/system_profiler", "SPDisplaysDataType", NULL};
            exec_cmd(buf, 1024, args);

            gpus[0] = strstr(buf, "Chipset Model: ");
            if(gpus[0] == 0)
                return 1;
            gpus[0] += 15;
            char *end = strchr(gpus[0], '\n');
            if(end == NULL)
                return 1;
            *end = 0;
        }
    #else
    # ifdef __ANDROID__
        return 1;
    # else
        get_gpus(gpus);
    
        // fallback (will only get 1 gpu)

        char gpu[256];
        if(gpus[0] == 0) {
            if(config.gpu_index > 1)   // lol why would you choose a non-existing GPU
                return 1;

            char *lspci = malloc(0x2000);
            char *args[] = {"lspci", "-mm", NULL};
            exec_cmd(lspci, 0x2000, args);

            gpus[0] = strstr(lspci, "3D");
            if(gpus[0] == 0) {
                gpus[0] = strstr(lspci, "VGA");
                if(gpus[0] == 0) {
                    free(lspci);
                    return 1;
                }
            }

            for(int j = 0; j < 4; ++j) {
                gpus[0] = strchr(gpus[0], '"');
                if(gpus[0] == 0) {
                    free(lspci);
                    return 1;
                }
                ++gpus[0];

                /* class" "manufacturer" "name"
                 *  "manufacturer" "name"
                 * manufacturer" "name"
                 *  "name"
                 * name"
                 */
            }

            end = strchr(gpus[0], '"');   // name
            if(end == NULL) {
                free(lspci);
                return 1;
            }
            *end = 0;
            strncpy(gpu, gpus[0], 255);
            free(lspci);
            gpus[0] = gpu;
        }
    # endif // __ANDROID__
    #endif // __APPLE__

    if(gpus[0] == NULL)
        return 1;

    // this next part is just random cleanup
    // also, I'm using end as a random char* - BaD pRaCtIcE aNd CoNfUsInG - lol stfu
    dest[0] = 0;    //  yk it's decent a yk it works
    for(unsigned j = 0; j < sizeof(gpus)/sizeof(gpus[0]) && gpus[j%3]; ++j) {
        if((_gpu_brand) == 0) {
            if(strstr(gpus[j], "Intel ")
               || strstr(gpus[j], "Apple "))
                gpus[j] += 6;
            else if(strstr(gpus[j], "AMD "))
                gpus[j] += 4;
        }

        if((end = strchr(gpus[j], '['))) {   // sometimes the gpu is "Architecture [GPU Name]"
            char *ptr = strchr(end, ']');
            if(ptr) {
                gpus[j] = end+1;
                *ptr = 0;
            }
        }
        
        if((end = strstr(gpus[j], " Integrated Graphics Controller")))
            *end = 0;
        if((end = strstr(gpus[j], " Rev. ")))
            *end = 0;

        // (finally) writing the GPUs into dest
        if(j > 0)
            strncat(dest, ", ", 256-strlen(dest));
        strncat(dest, gpus[j], 256-strlen(dest));
    }

    return 0;
}
