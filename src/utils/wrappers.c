#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "wrappers.h"
#include "return.h"

// Run a command using execvp and copy the output into buf
int execCmd(char *buf, size_t len, char *const *argv) {
    int stderr_pipes[2];
    int stdout_pipes[2];

    if(pipe(stdout_pipes) != 0 || pipe(stderr_pipes) != 0)
        return ERR_GENERIC;

    if(fork() == 0) {
        close(stdout_pipes[0]);
        close(stderr_pipes[0]);
        dup2(stdout_pipes[1], STDOUT_FILENO);
        dup2(stderr_pipes[1], STDERR_FILENO);

        execvp(argv[0], argv);
    }

    wait(0);

    close(stderr_pipes[0]);
    close(stderr_pipes[1]);

    close(stdout_pipes[1]);
    buf[read(stdout_pipes[0], buf, len) - 1] = 0;
    close(stdout_pipes[0]);

    return RET_OK;
}

// get the printed length of a string (not how big it is in memory)
__attribute__((pure)) size_t realStrlen(const char *str) {
    if(str == NULL)
        return RET_OK;

    size_t len = 0;

    bool escaping = false;

    // determine how long the printed string is (same logic as in printLine, utils.c)
    while(*str) {
        if(*str == '\n')
            break;

        // unicode continuation byte 0x10xxxxxx
        if(*str & 0x80 && (*str & 0x40) == 0) {
            ++str;
            continue;
        }

        if(*str != '\033') {
            len += (size_t)1 - escaping;

            escaping = (*str != 'm') && escaping;
        } else
            escaping = true;

        ++str;
    }

    return len;
}

// Copy no more than N characters of SRC to DEST, but always guarantee null-termination
void safeStrncpy(char *dest, const char *src, size_t n) {
    strncpy(dest, src, n - 1);
    dest[n - 1] = 0;
}
