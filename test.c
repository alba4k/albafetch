#include <stdio.h>
#include <sys/sysinfo.h>

int main() {
    struct sysinfo info;
    sysinfo(&info);

    unsigned long total = info.totalram / 1048576;
    unsigned long free = info.freeram / 1048576;

    printf("%lu %lu", total - free, total);
}
