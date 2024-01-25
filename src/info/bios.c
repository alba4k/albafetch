#include "info.h"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

// get the current BIOS vendor and version (Linux only!)
int bios(char *dest) {
    #ifdef __APPLE__
        (void)dest; // avoid unused parameter warning - lmao
        return 1;
    #else
    char *vendor = NULL, *version = NULL;
    FILE *fp = NULL;
    size_t len;

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_vendor", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        vendor = malloc(len);
        vendor[fread(vendor, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if((fp = fopen("/sys/devices/virtual/dmi/id/bios_version", "r"))) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        version = malloc(len);
        version[fread(version, 1, len, fp) - 1] = 0;

        fclose(fp);
    }

    if(vendor && version)
        snprintf(dest, 256, "%s %s", vendor, version);
    else if(vendor)
        strncpy(dest, vendor, 256);
    else if(version)
        strncpy(dest, version, 256);
    else
        return 1;

    free(vendor);
    free(version);
    
    return 0;
    #endif
}
