#include <stdio.h>
#include <string.h>
int main()
{
    char destination[] = "\e[36m";
    char *source = "\e[1m";
    strcat(destination,source);
    printf("Concatenated String: %s\n", destination);
    return 0;
}