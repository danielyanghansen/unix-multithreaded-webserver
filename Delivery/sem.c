#include "sem.h"
 
int run = 1;

SEM *sem_init(int initVal){
    SEM *sem = (SEM *) malloc(sizeof(SEM));
    
    sem->count = initVal;
    pthread_mutex_init(&sem->count_mutex, NULL);
    pthread_mutex_init(&sem->condition_mutex, NULL);
    pthread_cond_init(&sem->condition_cond,NULL);
    printf("Sem Init:\ninitVal: %d or %d", initVal, sem->count);

    return sem;
}

int sem_del(SEM *sem) {
    //wait for count to reach 0?
    free(sem);
    return 0;
}

void *V(SEM *sem){
    
    pthread_mutex_lock( &sem->count_mutex );
        sem->count++;
    pthread_mutex_unlock( &sem->count_mutex );

    int sigres;
    //Notify P operations: Unlock blocked thread operations
    pthread_mutex_lock( &sem->condition_mutex );
        sigres = pthread_cond_signal( &sem->condition_cond );
    pthread_mutex_unlock( &sem->condition_mutex );

    if(sigres != 0) {
        return (void *) errno;
    }

    return NULL;

}

void *P(SEM *sem) { //"Subprogram" Execution Thread
        
    while(run){
        int counterLog;
        int doTask = 0;

        pthread_mutex_lock( &sem->count_mutex );
            if (sem->count > 0) {
                sem->count--;
                counterLog = sem->count;
                doTask = 1;                    
            }
        pthread_mutex_unlock( &sem->count_mutex);
        if (doTask) {
            return NULL;
        }
        //If V signals in this "space", this specific P might Deadlock

        //Try to mitigate race condition by rechecking after a minute
        time_t now;
        struct timespec abstime;
        time(&now);
        abstime.tv_sec= now + 60;
        abstime.tv_nsec = 0;
        pthread_mutex_lock( &sem->condition_mutex ); //Wait until I get notified to try again
            pthread_cond_timedwait( &sem->condition_cond, &sem->condition_mutex, &abstime);
        pthread_mutex_unlock( &sem->condition_mutex );
    }
        
    //funksjonene bestemmer når på counten de kjører men det kan ikke være konstanter ettersom vi trenger å bruke de for flere
    //trenger også en måte å få størrelsen slik at vi kan vite hvilke størrelse listen skal være men 
}
void* produce(SEM *sem) {
    
    while (run) {
        printf("Adding 1 to the SEM counter...\n");
        void *res = V(sem);
        sleep(rand() % 3);
        //If hasJOb { put job on job queue (ringbuffer); V(sem)}
    }
    return NULL;
}
void *consume(SEM *sem) {
    while (run) {
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
/*
    pthread_create(&threads[0], NULL, &hello, p_sem);
    pthread_join(threads[0], NULL);
*/

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
/*
*/
    return 0;

}