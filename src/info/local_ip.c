#include "../config/config.h"
#include "info.h"

#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>

// get all local ips
SInfo *localIp(void) {
    SInfo *res = malloc(sizeof(SInfo));
    if(res == NULL)
        return NULL;
    res->string = NULL;
    res->retval = RET_OK;

    struct ifaddrs *addrs = NULL;
    bool done = false;

    getifaddrs(&addrs);

    struct ifaddrs *first = addrs;

    while(addrs) {
        // checking if the ip is valid
        if(addrs->ifa_addr && addrs->ifa_addr->sa_family == AF_INET) {
            // filtering out docker or localhost ips
            if((strcmp(addrs->ifa_name, "lo") != 0 || config.loc_localhost) && (strcmp(addrs->ifa_name, "docker0") != 0 || config.loc_docker)) {
                struct sockaddr_in *pAddr = (struct sockaddr_in *)addrs->ifa_addr;

                char buf[64] = "";
                snprintf(buf, sizeof(buf), "%s%s (%s)", done ? ", " : "", inet_ntoa(pAddr->sin_addr), addrs->ifa_name);
                char *tmpptr = realloc(res->string, (res->string ? strlen(res->string) : 0) + strlen(buf) + 1);
                if(tmpptr == NULL)
                    continue;

                if(res->string == NULL) // first time allocating
                    tmpptr[0] = 0;

                res->string = tmpptr;
                strcat(res->string, buf);
                done = true;
            }
        }

        addrs = addrs->ifa_next;
    }

    freeifaddrs(first);

    if(!done)
        res->retval = ERR_NO_INFO;

    return res;
}
