#ifndef server_h
#define server_h

#include "wrapper.h"


struct Server
{
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;
    struct sockaddr_in address;
    int socket;
};

struct Server server_init(int domain, int service, int protocol, u_long interface, int port, int backlog);

#endif
