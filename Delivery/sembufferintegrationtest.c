#include "sem.h"
#include "bbuffer.h"
#include "sem.c"
#include "bbuffer.c"

static int running = 1;

static BNDBUF * p_bndbuf;


void* produce(char* argv[]) {
    
    while (running) {
        //If hasJOb { put job on job queue (ringbuffer); V(sem)}
        int retval = bb_add(argv[1], rand() % 1024); //return value is equal to the random input value on success
        if (retval < 0){
            printf("There was an error\n most likely because of a full buffer");
            sleep(1);
            continue;
        } else {
            printf("Value added : %d\n", retval);
        }

        printf("Adding 1 to the SEM counter...\n");
        void *res = V(argv[0]);
        sleep(rand() % 3);
    }
    return NULL;
}
void *consume(char* argv[]) {
    while (running) {
        //waiting...
        printf("Starting to look for a job...\n");
        P(argv[0]); //waiting for signal...
        int retval = bb_get(argv[1]);
        //incredibly unlikely to return -1 because of mutex lock or simultaneous signaling, but still...:
        if (retval < 0) {
            printf("BIG ERROR WTF. ");
        }
        else {
        printf("Got the task number %d from the buffer\n", retval);
        printf("Removed 1 from the SEM counter!\nSleeping for ish 20 seconds on thread %ld to simulate working...\n", pthread_self());
        sleep(rand() % 20 + 10);
        printf("On thread %ld: Work (sleep) done with task %d!\n", pthread_self(), retval);
        }
        //Found something to do
        //Find job in ringbuffer and do it.
    }
    return NULL;
}



int main() {

    srand(time(NULL));

    printf("Main start\n");
    const SEM * p_sem = sem_init(0); //Init on how many pre-existing tasks there are
    const BNDBUF *p_bb = bb_init(10);
    p_bndbuf = bb_init(10);

    printf("Sem Created with threads\n");
    pthread_t consumers[NUM_THREADS];
    pthread_t producer;
    printf("Created consumer thread array with %d thread\n", NUM_THREADS);

    char *argv[] = {p_sem, p_bb};

    printf("Initiated producer thread\n");
    pthread_create(&producer, NULL, &produce, argv);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&consumers[i], NULL, &consume, argv);
    }
    printf("Inititated all consumer threads");

    void *retvalV = NULL;
    int joinResV = pthread_join(producer, &retvalV);

    for (int i = 0; i < NUM_THREADS; i++) {
        void *retval = NULL;
        int joinRes = pthread_join(consumers[i], &retval);
        printf("Return value %d: %d\n", i, joinRes);
    }
    printf("Joined all threads");
/*
*/
    return 0;
}
