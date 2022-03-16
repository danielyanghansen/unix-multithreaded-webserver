#ifndef Server_h
#define Server_h

#include <sys/socket.h>
#include <netinet/in.h>

//server properties
struct Server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address; //sockaddr_in6 for IPv6, unsure if current struct is cross compatible

    int socket;

    void (*launch)(struct Server *server);
};

//Forward declaration
struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port, int backlog, void(*launch)(struct Server *server));

#endif