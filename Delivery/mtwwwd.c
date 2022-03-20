#include "common.h"
#include <string.h> //For using strlen
//#include "sem.h"
//#include "bbuffer.h"


void handle_connection(int clientfd) {

    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actualpath[PATH_MAX +1];

    //read message from client -- (the pathname of the file for the server to return)
    while((bytes_read = read(clientfd, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0 ) {
        msgsize += bytes_read;
        if (msgsize < BUFSIZE -1 || buffer[msgsize-1] == '\n') break;
    }

    if (bytes_read < 0) {
        perror("recv error");
        exit(1);
    }
    buffer[msgsize-1] = 0; //null terminate message and remove linebreak

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);
    

    char line[sizeof(buffer)];
    strncpy(line, buffer[0], buffer[sizeof(buffer)-1]);
    printf("%s", line);

    //cleanup
    int size = strlen(buffer);
    if (buffer[size-1] == '\n' || buffer[size-1] == '\r') {
        buffer[size-1] = '\0';
    }
    if (buffer[size-2] == '\n' || buffer[size-2] == '\r') {
        buffer[size-2] = '\0';
    }

    //check valid path
    if (realpath(buffer, actualpath) == NULL) {
        printf("ERROR(bad path): %s\n", buffer);
        close(clientfd);    
        return;
    }

    //read file and send contents to client
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("ERROR(bad path): %s\n", buffer);
        close(clientfd);
        return;
    }

    //send file contents
    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0 ) {
        printf("sending %zu bytes\n", bytes_read);
        write(clientfd, buffer, bytes_read);
    }
    //close connection
    close(clientfd);
    fclose(fp);
    printf("closing connection...\n");

}

int main(int argc, char* argv, ...) {
    int listenfd, clientfd, addr_size;
    struct sockaddr_in servaddr, cliaddr;


    //set up socket and listen:
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    //setup server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
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

        printf("Waiting for connections...\n");
        //struct sockaddr_in addr;

        addr_size = sizeof(struct sockaddr_in);

        clientfd = accept(listenfd, (SA *) &cliaddr, (socklen_t*)&addr_size);

        if (clientfd == -1) {
            perror("Can't accept");
            continue;
        }
        printf("Found connection: %i \n", clientfd);

        handle_connection(clientfd);
    }

}