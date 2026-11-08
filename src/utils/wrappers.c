#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "return.h"
#include "wrappers.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Run a command using execvp and copy the output into buf
int execCmd(char *buf, size_t len, char *const *argv) {
    int stdout_pipes[2];

    if(pipe(stdout_pipes) != 0)
        return ERR_GENERIC;

    pid_t pid = fork();
    if(pid == 0) {
        close(stdout_pipes[0]);
        dup2(stdout_pipes[1], STDOUT_FILENO);

        int devnull = open("/dev/null", O_WRONLY);
        if(devnull != -1)
            dup2(devnull, STDERR_FILENO);

        execvp(argv[0], argv);

        exit(1); // in case execvp fails
    }

    close(stdout_pipes[1]);

    // the pipe has to be read before waiting, or a child filling it would block forever
    size_t total = 0;
    ssize_t rlen = 0;
    while(total < len && (rlen = read(stdout_pipes[0], buf + total, len - total)) > 0)
        total += (size_t)rlen;

    // closed before waiting, so that a child with more output than fits in buf gets SIGPIPE
    close(stdout_pipes[0]);
    waitpid(pid, NULL, 0);

    if(rlen < 0)
        return ERR_GENERIC;

    buf[total == 0 ? 0 : total - 1] = 0;
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
        if(*str & 0x80U && (*str & 0x40U) == 0) {
            ++str;
            continue;
        }

        if(*str != '\033') {
            len += (size_t)1 - escaping;

            escaping = (*str != 'm') && escaping;
        }
        else
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

// Check if a binary exists and is executable in PATH
bool binaryInPath(const char *binary) {
    if(!binary || !*binary)
        return false;

    if(strchr(binary, '/'))
        return access(binary, X_OK) == 0;

    const char *path = getenv("PATH");
    if(!path)
        return false;

    char full_path[PATH_MAX];
    const char *p = path;
    while(*p) {
        const char *next = strchr(p, ':');
        size_t dir_len = next ? (size_t)(next - p) : strlen(p);

        if(dir_len == 0)
            snprintf(full_path, sizeof(full_path), "./%s", binary);
        else
            snprintf(full_path, sizeof(full_path), "%.*s/%s", (int)dir_len, p, binary);

        if(access(full_path, X_OK) == 0)
            return true;

        if(!next)
            break;
        p = next + 1;
    }

    return false;
}
