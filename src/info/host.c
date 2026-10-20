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
SInfo *host(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifdef __APPLE__
    size_t BUF_SIZE = 256;
    char buf[256] = "";
    if(sysctlbyname("hw.model", buf, &BUF_SIZE, NULL, 0) != 0 || buf[0] == '\0') {
        res->retval = ERR_NO_INFO;
        return res;
    }

    res->string = strdup(buf);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }
#else
#ifdef __ANDROID__
    char brand[64] = "", model[64] = "";
    char *brand_args[] = {"getprop", "ro.product.brand", NULL};
    char *model_args[] = {"getprop", "ro.product.model", NULL};

    execCmd(brand, 64, brand_args);
    execCmd(model, 64, model_args);

    if(brand[0] == 0 || model[0] == 0) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    const size_t len = strlen(brand) + strlen(model) + (brand[0] ? 2 : 1);
    res->string = malloc(len);
    if(res->string == NULL) {
        res->retval = ERR_OOM;
        return res;
    }

    snprintf(res->string, len, "%s%s%s", brand, brand[0] ? " " : "", model);
#else
    char *name = NULL, *version = NULL;
    FILE *fp = NULL;
    size_t len;

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_name", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        name = malloc(len);
        if(name == NULL) {
            fclose(fp);
            goto skip_name;
        }

        size_t read = fread(name, sizeof(*name), len, fp);
        fclose(fp);
        if(read <= 0) {
            free(name);
            name = NULL;
            goto skip_name;
        }

        name[read - 1] = 0;
    }
skip_name:;

    if((fp = fopen("/sys/devices/virtual/dmi/id/product_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        version = malloc(len);
        if(version == NULL) {
            fclose(fp);
            goto skip_version;
        }

        size_t read = fread(version, sizeof(*version), len, fp);
        fclose(fp);
        if(read <= 0) {
            free(version);
            version = NULL;
            goto skip_version;
        }

        version[read - 1] = 0;
    }
skip_version:;

    // filtering out some shitty defaults because the file can't just be empty"
    const char *errors[] = {"System Product Name", "System Version", "To Be Filled By O.E.M.", "Default string-WCP", "None", ""};
    bool name_defined = (name != NULL);
    bool version_defined = (version != NULL);

    for(unsigned long i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i) {
        if(name && (strcmp(name, errors[i]) == 0))
            name_defined = false;
        if(version && (strcmp(version, errors[i]) == 0))
            version_defined = false;
    }

    if(name_defined && version_defined) {
        const size_t len = strlen(name) + strlen(version) + 2;
        res->string = malloc(len);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            goto free;
        }
        snprintf(res->string, len, "%s %s", name, version);

    free:
        free(name);
        free(version);
    }
    else if(name_defined) {
        free(version);
        res->string = name;
    }
    else if(version_defined) {
        free(name);
        res->string = version;
    }
    else {
        free(name);
        free(version);
        res->retval = ERR_NO_INFO;
    }
#endif // __ANDROID__
#endif // __APPLE__

    return res;
}
