#include "wrapper.h"

int Socket(int family, int type, int protocol)
{
    int n;
    if ((n = socket(family, type, protocol)) < 0)
    {
        perror("\nUnable to create socket...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}


int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int n;
    if(( n = bind(sockfd, addr, addrlen)) < 0)
    {
        perror("\nUnable to bind...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}


int Listen(int sockfd, int backlog_)
{
    int n;
    if ((n = listen(sockfd, backlog_)) < 0)
    {
        perror("\nUnable to listen...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}



int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int n;
    if ((n = connect(sockfd, addr, addrlen)) < 0)
    {
        perror("\nUnable to connect...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}


int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int n;
    if ((n = accept(sockfd, addr, addrlen)) < 0)
    {
        perror("\nUnable to accept...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}


int Inet_pton(int af, const char *src, void *dst)
{
    int n;
    if ((n = inet_pton(af, src, dst)) < 0)
    {
        perror("\nPTON error...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}


/*int Inet_ntop(int af, void *dst, const char *src, size_t size)
{
    int n;
    if((n = inet_ntop(af, dst, src, size)) < 0)
    {
        perror("\nNTOP error...\n");
        exit(EXIT_FAILURE);
    }
    return n;
}*/