#include <stdio.h>
#include <stdlib.h> 

int main() {
    char line[128];
    FILE *fp = fopen("albafetch.conf", "r");

    while(fscanf(fp, "%[^\n] ", line) != EOF) {
        if(line[0] == ';' || line[0] == '#')
            continue;
        puts(line);
    }
}