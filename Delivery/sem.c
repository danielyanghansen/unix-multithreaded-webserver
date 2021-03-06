#include "sem.h"
#include <unistd.h>

static int run = 1;

SEM *sem_init(int initVal){
    SEM *sem = (SEM *) malloc(sizeof(SEM));
    
    sem->count = initVal;
    pthread_mutex_init(&sem->count_mutex, NULL);
    pthread_mutex_init(&sem->condition_mutex, NULL);
    pthread_cond_init(&sem->condition_cond,NULL);

    return sem;
}

int sem_del(SEM *sem) {
    //wait for count to reach 0 ? (Maybe some time far into the future)
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

    return NULL;
        
    //funksjonene bestemmer når på counten de kjører men det kan ikke være konstanter ettersom vi trenger å bruke de for flere
    //trenger også en måte å få størrelsen slik at vi kan vite hvilke størrelse listen skal være men 
}
