#include <stdio.h>
#include <stdint.h>
int main() {
    static int num;
    static char used[] = "1000";
    static char total[] = "24000";

    num = (int) (intptr_t) used;

    printf("%i", num);
}
