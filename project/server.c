#include "server.h"

struct Server server_init(int domain, int service, int protocol, u_long interface, int port, int backlog)
{
    struct Server server;
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = Socket(domain, service, protocol);
    Bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address));
    Listen(server.socket, server.backlog);

    return server;
}
