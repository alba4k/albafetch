#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#else
#ifdef __ANDROID__
#include <unistd.h>
#include <sys/wait.h>

#include "../utils/wrappers.h"
#else
#include "../utils/wrappers.h"
#endif // __ANDROID__
#endif // __APPLE__

#include "info.h"

// get the machine name and eventually model version
int host(char *dest) {
#ifdef __APPLE__
    size_t BUF_SIZE = DEST_SIZE;
    sysctlbyname("hw.model", dest, &BUF_SIZE, NULL, 0);
#else
#ifdef __ANDROID__
    char brand[64], model[64];
    char *brand_args[] = {"getprop", "ro.product.brand", NULL};
    char *model_args[] = {"getprop", "ro.product.model", NULL};

    execCmd(brand, 64, brand_args);
    execCmd(model, 64, model_args);

    if((brand[0] || model[0]) == 0)
        return ERR_NO_INFO;

    snprintf(dest, DEST_SIZE, "%s%s%s", brand, brand[0] ? " " : "", model);
#else
    char *name = NULL, *version = NULL;
    FILE *fp = NULL;
    size_t len;

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        name = malloc(len);
        if(name == NULL)
            return ERR_OOM;
        name[fread(name, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        version = malloc(len);
        if(version == NULL)
            return ERR_OOM;
        version[fread(version, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    // filtering out some shitty defaults because the file can't just be empty"
    const char *errors[] = {"System Product Name", "System Version", "To Be Filled By O.E.M.", "None", ""};
    bool name_defined = true;
    bool version_defined = true;

    for(unsigned long i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i) {
        if(name)
            if(strcmp(name, errors[i]) == 0)
                name_defined = false;
        if(version)
            if(strcmp(version, errors[i]) == 0)
                version_defined = false;
    }

    if(name && version && name_defined && version_defined)
        snprintf(dest, DEST_SIZE, "%s %s", name, version);
    else if(name && name_defined)
        safeStrncpy(dest, name, DEST_SIZE);
    else if(version && version_defined)
        safeStrncpy(dest, version, DEST_SIZE);
    else {
        free(name);
        free(version);

        return ERR_NO_INFO;
    }

    free(name);
    free(version);
#endif // __ANDROID__
#endif // __APPLE__

    return RET_OK;
}
