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
        rewind(fp);

        vendor = malloc(len);
        if(vendor == NULL)
            return ERR_OOM;
        vendor[fread(vendor, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        version = malloc(len);
        if(version == NULL)
            return ERR_OOM;
        version[fread(version, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if(vendor != NULL && version != NULL)
        snprintf(dest, DEST_SIZE, "%s %s", vendor, version);
    else if(vendor != NULL)
        safe_strncpy(dest, vendor, DEST_SIZE);
    else if(version != NULL)
        safe_strncpy(dest, version, DEST_SIZE);
    else
        return ERR_NO_INFO;

    free(vendor);
    free(version);
    
    return RET_OK;
}
