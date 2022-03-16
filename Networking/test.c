#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Server.h"
int g_bufferSize = 30000;

void launch(struct Server *server) {
    
    char buffer[g_bufferSize];
    char *message = "HTTP/1.1 200OOK\nDate: Wed ,16 Mar 2022 11:00:00 GMT\nServer: Apaache/2.2.14 (Win32)\nLast-Modified: Wed, 16 Mar 2022 10:00:00 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n<html><body><h1>Hello World!</h1></body></html>";
    int address_length = sizeof(server->address);
    int new_socket;
    while (1) {
        //accept new socket connection
        printf("=============WAITING FOR CONNECTION=============\n");
        new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);
        read(new_socket, buffer, g_bufferSize);
        printf("%s\n", buffer);

        write(new_socket, message, strlen(message));

        close(new_socket);

    };
}

int main() {
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8081, 10, launch);
    server.launch(&server);
}