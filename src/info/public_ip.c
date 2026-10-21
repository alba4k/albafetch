#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../utils/wrappers.h"
#include "info.h"

// Also, this doesn't seem to be working with the Wifi of a Hotel I'm currently staying in
// I wonder why. There seems to be no IP adress in buf, but using curl works just fine.
// I should probably reimplement curl as optdep for fallback, idk

// get the current public ip
SInfo *publicIp(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    // https://stackoverflow.com/a/65362666 - thanks dbush

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    // using this as it is faster than ident.me
    if(getaddrinfo("whatismyip.akamai.com", "80", &hints, &addrs)) {
        res->retval = ERR_NO_INFO;
        return res;
    }

    int socket_fd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    if(socket_fd == -1) {
        freeaddrinfo(addrs);
        res->retval = ERR_NO_INFO + 0x10;
        return res;
    }

    if(connect(socket_fd, addrs->ai_addr, addrs->ai_addrlen) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        res->retval = ERR_NO_INFO + 0x20;
        return res;
    }

    char cmd[] = "GET / HTTP/1.1\nHost: whatismyip.akamai.com\n\n";
    if(send(socket_fd, cmd, strlen(cmd), 0) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        res->retval = ERR_NO_INFO + 0x30;
        return res;
    }

    char buf[1024] = "";
    if(recv(socket_fd, buf, sizeof(buf), 0) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        res->retval = ERR_NO_INFO + 0x40;
        return res;
    }

    close(socket_fd);
    freeaddrinfo(addrs);

    /* buf should now look like this:
     * """
     * HTTP/1.1 200 OK
     * [...]
     * [...]
     * 
     * 123.123.123.123
     * """
     */
    char *start = buf, *end;
    end = strchr(start, '\n');
    if(end == NULL) {
        res->retval = ERR_PARSING;
        return res;
    }

    if(strncmp(start, "HTTP/1.1 200 OK", end - start - 1) != 0) {
        res->retval = ERR_NO_INFO + 0x50;
        return res;
    }

    start = strstr(start, "\n\r\n");
    if(start == NULL) {
        res->retval = ERR_PARSING + 0x10;
        return res;
    }

    start += 3;

    res->string = strdup(start);
    if(res->string == NULL)
        res->retval = ERR_OOM;

    return res;
}
