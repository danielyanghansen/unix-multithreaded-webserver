#include "Server.h"
#include <stdio.h>
#include <stdlib.h>

struct Server server_constructor(
    int domain, 
    int service, 
    int protocol, 
    u_long interface, 
    int port, 
    int backlog, 
    void(*launch)(struct Server *server)) {
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

        //Setup socket connection
        server.socket = socket(domain, service, protocol); //Creates a socket connection to the network
        if (server.socket == 0) {
            perror("Socket connection failed...\n");
            exit(1);
        };

        //Bind socket to server
        if ((bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address))) < 0 ) {
            perror("Socket binding failed...\n");
            exit(1);
        };

        //setup listening
        if ((listen(server.socket, server.backlog)) < 0 ) {
            perror("Failed to start listening...\n");
            exit(1);
        };

        return server;
}
