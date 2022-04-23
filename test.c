#include <sys/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    struct sysinfo info;
    sysinfo(&info);

    unsigned long totalram = info.totalram;
    unsigned long freeram = info.freeram;
    unsigned long bufferram = info.bufferram;
    unsigned long sharedram = info.sharedram;

    char *str = malloc(0x1000);
    FILE *fp = fopen("/proc/meminfo", "r");     // open the file and copy its contents into str
    if(!fp) {
        fputs("[Missing /proc/meminfo]", stderr);
        fclose(fp);
        return -1;
    }
    str[fread(str, 1, 0x1000, fp)] = 0;
    fclose(fp);

    printf("Total: %lu\nFree: %lu\nBuffer: %lu\nShared: %lu\n", totalram/1024, freeram/1024, bufferram/1024, sharedram/1024);

    fprintf(stdout, "\n\n\n%s\n", str);

    free(str);

    return 0;
}