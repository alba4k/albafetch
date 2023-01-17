#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    FILE *fp = fopen("fakeconfig.conf", "r");
    
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char *conf = malloc(len+1);
    conf[fread(conf, 1, len, fp)] = 0;
    fclose(fp);

    char *ptr = strstr(conf, "modules");

    if(!ptr)
        return 1;

    ptr = strchr(ptr, '{');
    if(!ptr)
        return 1;
    char *start = ptr + 1;

    ptr = strchr (ptr, '}');
    if(!ptr)
        return 1;

    char *end = ptr;
    *end = 0;

    printf("%s\n", start);

    ptr = start;
    char *ptr2, *ptr3;
    while((ptr = strstr(ptr, "module"))) {
        ptr = strchr(ptr, '"');
        if(!ptr)
            return 1;
        ptr += 1;

        ptr2 = strchr(ptr, '"');
        if(!ptr2)
            return 1;
        *ptr2 = 0;
/*
        ptr3 = strchr(ptr2, ',');
        if(!ptr)
            return 1;
        *ptr3 = 0;
*/
        printf("%s\n", ptr);

        *ptr2 = '"';
        //*ptr3 = ',';
    }

    return 0;
}