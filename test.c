
#include <stdio.h>
#include <time.h>
#include <string.h>

int main () {
    char string[] = "abcde";
    printf("%s\n%d\n", string, strlen(string));
    string[strlen(string) -1] = 0;
    printf("\n%s\n", string);
}