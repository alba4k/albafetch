#include "info.h"

#include <string.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

// get the current public ip
int public_ip(char *dest) {
    // https://stackoverflow.com/a/65362666 - thanks dbush

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    
    // using this as it is faster than ident.me
    if(getaddrinfo("whatismyip.akamai.com", "80", &hints, &addrs))
        return 1;

    int socket_fd = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    if(socket_fd == -1)
        goto error;
        
    if(connect(socket_fd, addrs->ai_addr, addrs->ai_addrlen) == -1)
        goto error;

    char cmd[] = "GET / HTTP/1.1\nHost: whatismyip.akamai.com\n\n";
    if(send(socket_fd, cmd, strlen(cmd), 0) == -1)
        goto error;

    char buf[1024] = {0};
    if(recv(socket_fd, buf, sizeof(buf), 0) == -1)
        goto error;

    close(socket_fd);

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
        goto error;


    start = strstr(start, "\n\r\n");
    if(start == NULL)
        goto error;

    start += 3;
    
    strncpy(dest, start, 256);

    freeaddrinfo(addrs);
    return 0;

    error:
    freeaddrinfo(addrs);
    return 1;
}
