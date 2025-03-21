#include <string.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

#include "info.h"
#include "../utils/wrappers.h"

// Also, this doesn't seem to be working with the Wifi of a Hotel I'm currently staying in
// I wonder why. There seems to be no IP adress in buf, but using curl works just fine.
// I should probably reimplement curl as optdep for fallback, idk
int puts(const char *);
// get the current public ip
int public_ip(char *dest) {
    // https://stackoverflow.com/a/65362666 - thanks dbush

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    
    // using this as it is faster than ident.me
    if(getaddrinfo("whatismyip.akamai.com", "80", &hints, &addrs)) {
        return ERR_NO_INFO;
    }

    int socket_fd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    if(socket_fd == -1) {
        freeaddrinfo(addrs);
        return ERR_NO_INFO + 0x10;
    }
        
    if(connect(socket_fd, addrs->ai_addr, addrs->ai_addrlen) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        return ERR_NO_INFO + 0x20;
    }

    char cmd[] = "GET / HTTP/1.1\nHost: whatismyip.akamai.com\n\n";
    if(send(socket_fd, cmd, strlen(cmd), 0) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        return ERR_NO_INFO + 0x30;
    }

    char buf[1024] = "";
    if(recv(socket_fd, buf, sizeof(buf), 0) == -1) {
        close(socket_fd);
        freeaddrinfo(addrs);
        return ERR_NO_INFO + 0x40;
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
    if(strncmp(start, "HTTP/1.1 200 OK", end - start - 1))
        return ERR_NO_INFO + 0x50;

    start = strstr(start, "\n\r\n");
    if(start == NULL)
        return ERR_PARSING;

    start += 3;
    
    safe_strncpy(dest, start, DEST_SIZE);

    return RET_OK;
}
