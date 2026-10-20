#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "info.h"
#include "../utils/wrappers.h"

// get the current BIOS vendor and version (Linux only!)
SInfo *bios(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

#ifndef __linux__
    res->retval = ERR_UNSUPPORTED;
#else
    char *vendor = NULL, *version = NULL;
    FILE *fp = NULL;
    size_t len;

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        vendor = malloc(len);
        if(vendor == NULL)
            goto skip_vendor;

        size_t read = fread(vendor, sizeof(*vendor), len, fp);
        fclose(fp);
        if(read > 0)
            vendor[read - 1] = 0;
        else {
            free(vendor);
            vendor = NULL;
        }
    }
skip_vendor:;

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        version = malloc(len);
        if(version == NULL)
            goto skip_version;

        size_t read = fread(version, sizeof(*version), len, fp);
        fclose(fp);
        if(read > 0)
            version[read - 1] = 0;
        else {
            free(version);
            version = NULL;
        }
    }
skip_version:;

    if(vendor != NULL && version != NULL) {
        const size_t len = strlen(vendor) + strlen(version) + 2;
        res->string = malloc(len);
        if(res->string == NULL) {
            res->retval = ERR_OOM;
            goto free;
        }
        snprintf(res->string, len, "%s %s", vendor, version);

    free:
        free(vendor);
        free(version);
    }
    else if(vendor != NULL)
        res->string = vendor;
    else if(version != NULL)
        res->string = version;
    else
        res->retval = ERR_NO_INFO;
#endif // __linux__

    return res;
}
