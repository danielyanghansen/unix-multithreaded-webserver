#include "bbuffer.h"

BNDBUF *bb_init(unsigned int size){

    //Malloc is often considered somewhat expensive. (Avoid additional/excess malloc)
    BNDBUF *bbuff = (BNDBUF *) malloc(sizeof(BNDBUF) + sizeof(int) * (size -1));
    //Because the int array value[] is at the end of the struct, all of the free space at the end of the malloc becomes the list
    bbuff->next_in = 0;
    bbuff->next_out = 0;
    bbuff->count = 0;
    bbuff->bufsiz = size;

    for (int i = 0; i < bbuff->bufsiz; i++) {
        bbuff->value[i] = -1;
    }

    pthread_mutex_init(&bbuff->buf_mutex, NULL);

    return bbuff;
}

void bb_del(BNDBUF *bb) {  
    free(bb);
}



int bb_get(BNDBUF *bb){ //is called by consumer. Returns -1 of the queue is empty
    //returns -1 if the queue is empty

    int returnValue = -1;


    pthread_mutex_lock(&bb->buf_mutex);
    if (bb->count > 0) {
        bb->count--;
        returnValue = bb->value[bb->next_out];

        bb->value[bb->next_out] = -1; //Reset

        bb->next_out = (bb->next_out +1) % bb->bufsiz; //wrap around
    }
    pthread_mutex_unlock(&bb->buf_mutex);

    //return job
    return returnValue;
    
}

int bb_add(BNDBUF *bb, int fd) { //is called by producer. In our case, fd is the socket descriptor for the incoming request
        //returns -1 if the queue is full

    int returnValue = -1;
    pthread_mutex_lock(&bb->buf_mutex);

    if (bb->count < bb->bufsiz) {
        bb->count++;
        bb->value[bb->next_in] = fd;
        returnValue = fd;

        bb->next_in = (bb->next_in +1) % bb->bufsiz; //wrap around
    }    
    pthread_mutex_unlock(&bb->buf_mutex);

    //return file descriptor
    return returnValue;
}
