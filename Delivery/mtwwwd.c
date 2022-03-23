#include "common.h"
#include <string.h> //For using strlen
#include "sem.h"
#include "sem.c"
#include "bbuffer.h"
#include "bbuffer.c"

int running = 1;

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

void *produce(char* argv[]) {
    int addr_size;
    struct sockaddr_in cliaddr;

    int listenfd = argv[3]; //= ? Needs to be passed from main
    while (*argv[2] == 1) {

        printf("Waiting for connections...\n");

        addr_size = sizeof(struct sockaddr_in);

        int clientfd = accept(listenfd, (SA *) &cliaddr, (socklen_t*)&addr_size); //note: can return 0 as a valid entry
        if (clientfd == -1) {
            perror("Can't accept");
            continue;
        }
        printf("Found connection: %i \n", clientfd);

        //Try to put job on ringbuffer
        int retval = bb_add(argv[1],clientfd); //return value is equal to the random input value on success
        if (retval < 0){
            printf("There was an error\n most likely because of a full buffer.\n Closing connection...\n");
            close(clientfd);
            continue;
        } else {
            printf("Value added : %d\n", retval);
        }
        void *res = V(argv[0]);
        if (( void* )res != NULL) {
            return res;
        }
    }
    return NULL;
}

void *consume(char* argv[]) {
    //while (run = 1)
    printf("Thread ID: %d\n", pthread_self());
    while (*argv[2] == 1) {

        P(argv[0]);
        int clientfd = bb_get(argv[1]);
        if (clientfd < 0) {
            printf("Couldn't fetch task from ringbuffer...\n");
        }
        else {
            handle_connection(clientfd);
        }
    }

}


int main(int argc, char* argv, ...) {
    int listenfd;
    struct sockaddr_in servaddr;
    int* running = malloc(sizeof(int));
    memset(running, 1, sizeof(*running));

    //CREATE SEM AND BUFFER

    BNDBUF* bb = bb_init(BUFSIZ);
    //TODO: ? add back orphaned issues into buffer

    //Check current task count in buffer (expected to be 0)
    int initialCount = bb->count;

    SEM* sem = sem_init(initialCount);



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

    //Create threads
    pthread_t consumers[NUM_THREADS];
    pthread_t producer;
    char* argvs[] = {sem, bb, running, listenfd};

    //Setup worker threads (will default into condition-wait)
    for (int i = 0; i < NUM_THREADS; i++) {
            pthread_create(&consumers[i], NULL, &consume, argvs);
    }
    printf("Created %d worker threads.\n", NUM_THREADS);

    pthread_create(&producer, NULL, &produce, argvs);
    printf("Created producer thread\n");

    while(*running) {

    }

    printf("Joining threads...\n");
    void *retvalV = NULL;
    int joinResV = pthread_join(producer, &retvalV);

    for (int i = 0; i < NUM_THREADS; i++) {
        void *retval = NULL;
        int joinRes = pthread_join(consumers[i], &retval);
        printf("Return value %d: %d\n", i, joinRes);
    }
    printf("Joined all threads\n");

    return 0;

}