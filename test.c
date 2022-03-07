#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/sysinfo.h>

int main() {
    
    struct sysinfo info;
    sysinfo(&info);
    unsigned long tot = info.totalram;
    unsigned long used = (tot - info.sharedram - info.freeram - info.bufferram)/1048576;
    printf("%lu MiB / %lu MiB\n",used, tot/1048576);
}
