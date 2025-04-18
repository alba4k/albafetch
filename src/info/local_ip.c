#include "info.h"
#include "../config/config.h"

#include <string.h>

#include <ifaddrs.h>
#include <stdio.h>
#include <arpa/inet.h>

// get all local ips
int localIp(char *dest) {
    struct ifaddrs *addrs = NULL;
    bool done = false;
    int buf_size = DEST_SIZE;

    getifaddrs(&addrs);

    struct ifaddrs *first = addrs;

    while(addrs) {
        // checking if the ip is valid
        if(addrs->ifa_addr && addrs->ifa_addr->sa_family == AF_INET) {
            // filtering out docker or localhost ips
            if((strcmp(addrs->ifa_name, "lo") != 0 || _loc_localhost) && (strcmp(addrs->ifa_name, "docker0") != 0 || _loc_docker)) {
                struct sockaddr_in *pAddr = (struct sockaddr_in *)addrs->ifa_addr;

                // saving it to the list of interfaces
                snprintf(dest, buf_size, "%s%s (%s)", done ? ", " : "", inet_ntoa(pAddr->sin_addr), addrs->ifa_name);
                dest += strlen(dest);
                buf_size -= strlen(dest);
                done = true;
            }
        }

        addrs = addrs->ifa_next;
    }

    freeifaddrs(first);

    if(done)
        return RET_OK;
    else
        return ERR_NO_INFO;
}
