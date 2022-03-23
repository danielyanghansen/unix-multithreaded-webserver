#include <string.h> //For using strlen
#include "common.h"
#include "sem.h"
#include "sem.c"
#include "bbuffer.h"
#include "bbuffer.c"

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_access_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MAXPATHSIZ 1000
#define ADDITIONAL_PATH_MAX 512
int running = 1;
char* basePath[MAXPATHSIZ] = {'\0'};

void handle_connection(int clientfd) {

    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;
    char additionalpath[ADDITIONAL_PATH_MAX + 1];
    char fullpath[MAXPATHSIZ + ADDITIONAL_PATH_MAX + 1];
    int posEndBasePath = strlen(basePath);

    
    pthread_mutex_lock(&buffer_mutex);
        //read message from client (HTTP)
        while((bytes_read = read(clientfd, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0 ) {
            msgsize += bytes_read;
            if (msgsize < BUFSIZE -1 || buffer[msgsize-1] == '\n') break;
        }
    pthread_mutex_unlock(&buffer_mutex);

    if (bytes_read < 0) {
        perror("recv error");
        exit(1);
    }
    //====================HTTTP PARSING====================

    const char s[2] = {' '};
    char* token;

    //Get the first token
    token = strtok(buffer, s);

    //walk through the other tokens
    
    char method[32] = {'\0'};
    strcpy(&method, token);
    token = strtok(NULL, s);

    char path[MAXPATHSIZ + ADDITIONAL_PATH_MAX + 1] = {'\0'};
    strcpy(&path, token);
    token = strtok(NULL, s);

    char HTTPver[64] = {'\0'};
    strcpy(&HTTPver, token);
    token = strtok(NULL, s);

    while( token != NULL ) {
      printf( "Additional unlogged header:\n    %s\n", token );
    
      token = strtok(NULL, s);
   }

    /* Get the right amount of memory */
    /*char path[end_of_path - start_of_path ];
    char HTTPver[&buffer[strlen(buffer) -1 ]- (end_of_path +1 )];
    */

    /* Copy the strings into our memory */
    /*
    strncpy(path, start_of_path,  end_of_path - start_of_path);
    strncpy(method, &buffer[0], start_of_path-&buffer[0]-1);
    strncpy(HTTPver, end_of_path+2, &buffer[strlen(buffer) -1]- (end_of_path + 2));
    */
    /* Null terminators (because strncpy does not provide them) */

    printf("Method: %s\n", method);
    printf("Sizeof method: %i\n", (int) sizeof(method));
    printf("HTTPVer: %s\n", HTTPver);
    printf("Sizeof HTTPver: %i\n", (int) sizeof(HTTPver));

    //8===========================================================D

    strcat(additionalpath, path); //FOR TESTING
    buffer[msgsize-1] = 0; //null terminate message and remove linebreak

    printf("REQUEST: %s\n", buffer); //The full request is in the buffer
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


    strcat(fullpath, basePath);
    printf("Full Path after first cat: %s\n", fullpath);
    strcat(fullpath, additionalpath);
    printf("Full Path after second cat: %s\n", fullpath);

    //check valid path (if the path length is short enough)
    if (realpath(fullpath, fullpath) == NULL) {
        printf("ERROR(bad fullpath): %s\n", additionalpath);
        close(clientfd);    
        return;
    }
    printf("Full Path after cat: %s\n", fullpath);
    
    //read file and send contents to client
    pthread_mutex_lock(&file_access_mutex);
        FILE *fp = fopen(fullpath, "r");
        if (fp == NULL) {
            printf("ERROR(bad path): %s\n", buffer);
            close(clientfd);
            pthread_mutex_unlock(&file_access_mutex);
            return;
        }

        pthread_mutex_lock(&buffer);
            //send file contents
            while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0 ) {
                printf("sending %zu bytes\n", bytes_read);
                write(clientfd, buffer, bytes_read);
            }
        pthread_mutex_unlock(&buffer);

    pthread_mutex_unlock(&file_access_mutex);

    //close connection
    close(clientfd);
    fclose(fp);
    printf("closing connection...\n");

}

void *produce(char* thread_args[]) {
    int addr_size;
    struct sockaddr_in cliaddr;

    int listenfd = thread_args[3]; //= ? Needs to be passed from main
    while (*thread_args[2] == 1) {

        printf("Waiting for connections...\n");

        addr_size = sizeof(struct sockaddr_in);

        int clientfd = accept(listenfd, (SA *) &cliaddr, (socklen_t*)&addr_size); //note: can return 0 as a valid entry
        if (clientfd == -1) {
            perror("Can't accept");
            continue;
        }
        printf("Found connection: %i \n", clientfd);

        //Try to put job on ringbuffer
        int retval = bb_add(thread_args[1],clientfd); //return value is equal to the random input value on success
        if (retval < 0){
            printf("There was an error\n most likely because of a full buffer.\n Closing connection...\n");
            close(clientfd);
            continue;
        } else {
            printf("Value added : %d\n", retval);
        }
        void *res = V(thread_args[0]);
        if (( void* )res != NULL) {
            return res;
        }
    }
    return NULL;
}

void *consume(char* thread_args[]) {
    //while (run = 1)
    printf("Thread ID: %d\n", pthread_self());
    while (*thread_args[2] == 1) {

        P(thread_args[0]);
        int clientfd = bb_get(thread_args[1]);
        if (clientfd < 0) {
            printf("Couldn't fetch task from ringbuffer...\n");
        }
        else {
            handle_connection(clientfd);
        }
    }

}


int main(int argc, char **argv) {
    // ./mtwwwd www-path port #threads #bufferslots
    if (argc != 5) {
        printf("Not enough args\n");
        printf("Needs 4 arguments: www-path port #threads #bufferslots\n");
        return 0;
    }

    char* www_path[MAXPATHSIZ -1] = {'\0'};
    strcpy(&www_path, argv[1]);
    strcpy(&basePath, argv[1]);

    int port = atoi(argv[2]);
    int threadsnr = atoi(argv[3]);
    int bufferslots = atoi(argv[4]);
    

    int listenfd;
    struct sockaddr_in servaddr;
    int* running = malloc(sizeof(int));
    memset(running, 1, sizeof(*running));

    printf("Attempting to set up server:\n    path: %s\n    port: %d\n    threadcount: %d\n    buffersize: %d\n", www_path, port, threadsnr, bufferslots);


    BNDBUF* bb = bb_init(bufferslots);
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
    servaddr.sin_port = htons(port);

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
    printf("Started listening on port %i...\n", port);

    //Create threads
    pthread_t consumers[threadsnr];
    pthread_t producer;
    char* thread_args[] = {sem, bb, running, listenfd};

    //Setup worker threads (will default into condition-wait)
    for (int i = 0; i < threadsnr; i++) {
            pthread_create(&consumers[i], NULL, &consume, thread_args);
    }
    printf("Created %d worker threads.\n", threadsnr);

    pthread_create(&producer, NULL, &produce, thread_args);
    printf("Created producer thread\n");

    while(*running) {

    }

    printf("Joining threads...\n");
    void *retvalV = NULL;
    int joinResV = pthread_join(producer, &retvalV);

    for (int i = 0; i < threadsnr; i++) {
        void *retval = NULL;
        int joinRes = pthread_join(consumers[i], &retval);
        printf("Return value %d: %d\n", i, joinRes);
    }
    printf("Joined all threads\n");

    return 0;

}