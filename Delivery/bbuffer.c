#include "bbuffer.h"



BNDBUF *bb_init(unsigned int size){
    BNDBUF *bbuff = (BNDBUF *) {size, 0 ,0};

    return bbuff;
}
void bb_del(BNDBUF *bb) {  

}



int  bb_get(BNDBUF *bb){ //is called by consumer. Returns -1 of the queue is empty
    int a = 1; //assuming there's no value to call
    //check if empty
    for (int i = 0; i <= sizeof(bb->value) -1; i++) {
        if (!(bb->value[i])) continue; //if spot is empty
        else {
            a = 0;
            break;
        }

    }
    //returns -1 if the queue is empty
    if (a || bb->value[bb->next_out] == (void *) 0) {
        return -1;
    }
    int returnValue = bb->value[bb->next_out];

    //remove returnValue from queue
    bb->value[bb->next_out] = (void *) 0;

    //increment queue in
    if (bb->next_in >= sizeof(bb->value) -1 ) {
        bb->next_in = 0;
    }
    else bb->next_in++;

    //return job
    return returnValue;
    
}

int bb_add(BNDBUF *bb, int fd) { //is called by producer. In our case, fd is the socket descriptor for the incoming request
    int a = 1;  //assuming there's no empty spots
    //check if the queue is full
    for (int i = 0; i <= sizeof(bb->value) -1; i++) {
        if (bb->value[i]) continue; //if spot is filled
        else {
            a = 0;
            break;
        }
    }
    //returns -1 if the queue is full
    if (a || bb->value[bb->next_in]) {
        return -1;
    }
       
    //add fd (job) to queue spot.
    bb->value[bb->next_in] = fd;
    //increment bb->next_out

    if (bb->next_out >= sizeof(bb->value) -1 ) {
        bb->next_out = 0;
    }
    else bb->next_out ++;

    return fd;
}
