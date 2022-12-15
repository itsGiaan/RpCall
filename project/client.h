#ifndef client_h
#define client_h

#include "wrapper.h"

struct Client
{
    int domain;
    int service;
    int protocol;
    //u_long interface;
    //int port;
    //struct sockaddr_in address;
    int socket;
};

struct Client client_init(int domain, int service, int protocol);

#endif