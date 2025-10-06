#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "info.h"
#include "../utils/wrappers.h"

// get the current BIOS vendor and version (Linux only!)
int bios(char *dest) {
    char *vendor = NULL, *version = NULL;
    FILE *fp = NULL;
    size_t len;

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        vendor = malloc(len);
        if(vendor == NULL)
            return ERR_OOM;
        
        size_t read = fread(vendor, sizeof(*vendor), len, fp);
        fclose(fp);
        if(read > 0)
            vendor[read - 1] = 0;
        else {
            free(vendor);
            vendor = NULL;
        }
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        version = malloc(len);
        if(version == NULL) {
            free(vendor);
            return ERR_OOM;
        }
        
        size_t read = fread(version, sizeof(*version), len, fp);
        fclose(fp);
        if(read > 0)
            version[read - 1] = 0;
        else {
            free(version);
            free(vendor);
            version = NULL;
        }
    }

    if(vendor != NULL && version != NULL)
        snprintf(dest, DEST_SIZE, "%s %s", vendor, version);
    else if(vendor != NULL)
        safeStrncpy(dest, vendor, DEST_SIZE);
    else if(version != NULL)
        safeStrncpy(dest, version, DEST_SIZE);
    else
        return ERR_NO_INFO;

    free(vendor);
    free(version);

    return RET_OK;
}
