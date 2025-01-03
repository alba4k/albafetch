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
#ifndef __ANDROID__
#include "../optdeps/optdeps.h"
#endif // __ANDROID__
#endif // __APPLE__

// get the gpu name(s)
int gpu(char *dest) {
    char *gpus[] = {NULL, NULL, NULL};

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
    #ifdef __ANDROID__
        return 1;
    #else
        get_gpus(gpus);
    #endif // __ANDROID__
    #endif // __APPLE__

    if(gpus[0] == NULL)
        return 1;

    // this next part is just random cleanup
    // also, I'm using end as a random char* - BaD pRaCtIcE aNd CoNfUsInG - lol stfu
    //  yk it's decent and yk it works
    dest[0] = 0;
    for(unsigned i = 0; i < sizeof(gpus)/sizeof(gpus[0]) && gpus[i] != NULL; ++i) {
        if((_gpu_brand) == 0) {
            if(strstr(gpus[i], "Intel ")
               || strstr(gpus[i], "Apple "))
                gpus[i] += 6;
            else if(strstr(gpus[i], "AMD "))
                gpus[i] += 4;
        }

        char *end = strchr(gpus[i], '[');
        if(end) {   // sometimes the gpu is "Architecture [GPU Name]"
            char *ptr = strchr(end, ']');
            if(ptr) {
                gpus[i] = end+1;
                *ptr = 0;
            }
        }
        
        if((end = strstr(gpus[i], " Integrated Graphics Controller")))
            *end = 0;
        if((end = strstr(gpus[i], " Rev. ")))
            *end = 0;

        // (finally) writing the GPU(s) into dest
        if(i > 0)
            strncat(dest, ", ", DEST_SIZE-strlen(dest));
        strncat(dest, gpus[i], DEST_SIZE-1-strlen(dest));
    }

    return 0;
}
