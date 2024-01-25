#include "info.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif // __APPLE__
#ifdef __ANDROID__
#include <unistd.h>
#include <sys/wait.h>
#endif // __ANDROID__

// get the machine name and eventually model version
int host(char *dest) {
    #ifdef __APPLE__
        size_t BUF_SIZE = 256;
        sysctlbyname("hw.model", dest, &BUF_SIZE, NULL, 0);
    #else
    #ifdef __ANDROID__
        int pipes[2];
        char brand[64], model[64];

        if(pipe(pipes))
            return 1;
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("getprop", "getprop", "ro.product.brand", NULL); 
        }

        wait(0);
        close(pipes[1]);
        brand[read(pipes[0], brand, 64) - 1] = 0;
        close(pipes[0]);

        if(pipe(pipes))
            return 1;
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("getprop", "getprop", "ro.product.model", NULL); 
        }

        wait(0);
        close(pipes[1]);
        model[read(pipes[0], model, 64) - 1] = 0;
        close(pipes[0]);

        if(!(brand[0] || model[0]))
            return 1;

        snprintf(dest, 256, "%s%s%s", brand, brand[0] ? " ": "", model);
    #else
        char *name = NULL, *version = NULL;
        FILE *fp = NULL;
        size_t len;

        if((fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r"))) {
            fseek(fp, 0, SEEK_END);
            len = ftell(fp);
            rewind(fp);

            name = malloc(len);
            name[fread(name, 1, len, fp) - 1] = 0;
            
            fclose(fp);
        }
       
        if((fp = fopen("/sys/devices/virtual/dmi/id/product_version", "r"))) {
            fseek(fp, 0, SEEK_END);
            len = ftell(fp);
            rewind(fp);

            version = malloc(len);
            version[fread(version, 1, len, fp) - 1] = 0;

            fclose(fp);
        }

        // filtering out some shitty defaults because the file can't just be empty
        const char *errors[] = {"System Product Name", "System Version", "To Be Filled By O.E.M.", ""};
        bool name_defined = true;
        bool version_defined = true;

        for(unsigned long i = 0; i < sizeof(errors)/sizeof(errors[0]); ++i) {
            if(name)
                if(!strcmp(name, errors[i]))
                    name_defined = false;
            if(version)
                if(!strcmp(version, errors[i]))
                    version_defined = false;
        }

        if(name && version && name_defined && version_defined)
            snprintf(dest, 256, "%s %s", name, version);
        else if(name && name_defined)
            strncpy(dest, name, 256);
        else if(version && version_defined)
            strncpy(dest, version, 256);
        else
            return 1;

        free(name);
        free(version);
    #endif // __ANDROID__
    #endif // __APPLE__

    return 0;
}
