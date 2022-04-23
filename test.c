#include <stdio.h>

int main() {
    char *a = "idk, i guess %s";
    printf("%s", a, "this won't work");
}