#include "client.h"

struct Client client_init(int domain, int service, int protocol)
{
    struct Client client;
    client.domain = domain;
    client.service = service;
    client.protocol = protocol;
    //client.interface = interface;
    //client.port = port;
    //client.address.sin_family = domain;
    //client.address.sin_port = htons(port);
    //client.address.sin_addr.s_addr = htonl(interface);
    client.socket = Socket(domain, service, protocol);
    return client;
}
