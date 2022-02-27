#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fpointer = fopen("/etc/hostname", "r");

    if(fpointer == NULL) {
        printf("Error! The file does not exist.");
        exit(0);
    }
    static char text[33];
    fgets(text, 33, fpointer);
    printf("%s", text);

    fclose(fpointer);
}