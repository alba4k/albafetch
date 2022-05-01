#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>

int main() {
    if(!access("/usr/bin/albafetch", F_OK)) {
        printf("shit\n");
    }
    if(!access("/usr/bin/snap", F_OK)) {
        printf("SHIT\n");
    }
}