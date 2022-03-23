#include "sem.h"
#include "sem.c"


static int running = 1;
/*


*/void* produce(SEM *sem) {
    
    while (running) {
        printf("Adding 1 to the SEM counter...\n");
        void *res = V(sem);
        sleep(rand() % 3);
        //If hasJOb { put job on job queue (ringbuffer); V(sem)}
    }
    return NULL;
}
void *consume(SEM *sem) {
    while (running) {
        //waiting...
        printf("Starting to look for a job...\n");
        P(sem);
        printf("Found something and removed 1 from the SEM counter!\nSleeping for 10 seconds on thread %ld to simulate working...\n", pthread_self());
        sleep(rand() % 8 + 4);
        printf("On thread %ld: Work (sleep) done!\n", pthread_self());
        //Found something to do
        //Find job in ringbuffer and do it.
    }
    return NULL;
}




int main() {

    srand(time(NULL));

    printf("Main start\n");
    const SEM * p_sem = sem_init(0); //Init on how many pre-existing tasks there are

    printf("Sem Created with threads\n");
    pthread_t consumers[NUM_THREADS];
    pthread_t producer;
    printf("Created thread array\n");

    pthread_create(&producer, NULL, &produce, p_sem);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&consumers[i], NULL, &consume, p_sem);
    }
    printf("Filled thread array\n");

    void *retvalV = NULL;
    int joinResV = pthread_join(producer, &retvalV);

    for (int i = 0; i < NUM_THREADS; i++) {
        void *retval = NULL;
        int joinRes = pthread_join(consumers[i], &retval);
        printf("Return value %d: %d\n", i, joinRes);
    }
    printf("Joined all threads");
    return 0;

}