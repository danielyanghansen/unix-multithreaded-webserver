#include "common.h"
#include <string.h> //For using strlen

int main(int argc, char* argv, ...) {
    int listenfd, clientfd;
    struct sockaddr_in servaddr, cliaddr;
    uint8_t buffer[MAXLINE +1];
    uint8_t recvline[MAXLINE +1];


    //set up socket and listen:
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    //setup server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    //bind port
    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0 ) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    //start listening
    if ((listen(listenfd, MAX_PENDING_CONNECTIONS)) != 0 ) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    printf("Started listening on port %i...\n", SERVER_PORT);

    //MAIN LOOP; ACCEPT AND HANDLE SOCKET CONNECTIONS

    while(1) {
        //struct sockaddr_in addr;
        clientfd = accept(listenfd, (SA *) &cliaddr, (socklen_t) sizeof(cliaddr));

        if (clientfd == -1) {
            perror("Can't accept");
            continue;
        }
        printf("Found connection: %i \n", clientfd);

        char response[] = "Hello World!"; //modify later

        write(clientfd, response, sizeof(response)-1);
        close(clientfd);
    }

}