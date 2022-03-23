#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "sem.h"
#include "sem.c"

#define NUM_THREADS 1000
#define NUM_SIMULTANIOUS 423

SEM sem;

char buf[BUFSIZ];

void *thread_work(void *arg)
{
    // wait
    printf("Thread %ld wait\n", (long int) arg);
    P(sem);
    printf("Thread %ld run\n", (long int) arg);

    // critical section
    sleep(rand() % 5);

    // signal
    V(sem);
    return 0;
}

int main()
{
    srand(time(NULL));

    setvbuf(stdout, buf, _IOLBF, BUFSIZ);
    const SEM *p_sem = sem_init(NUM_SIMULTANIOUS);
    sem = *p_sem;
    pthread_t thread[NUM_THREADS];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (long int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread[i], &attr, thread_work, (void *) i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        void * retval;
        pthread_join(thread[i], &retval);
        printf("Return value %d: %ld\n", i, (long int) retval);
    }
    
    sem_del(&sem);
    return 0;
}