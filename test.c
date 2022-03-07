#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main() {
    int pipes[2];
    char used[9];
    pipe(pipes);
    pipe(pipes);
    if(!fork()) {
        close(pipes[0]);
        dup2(pipes[1], STDOUT_FILENO);

        system("free --mebi | grep M | awk '{print $3}'");
        exit(0);
    }
    wait(NULL);
    close(pipes[1]);

    int len = read(pipes[0], used, 9);
    used[len - 1] = 0;

    printf("1234%s1234\n", used);
}
