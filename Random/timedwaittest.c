#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/queue.h>

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

int main() {
    time_t now;
    struct timespec abstime;
    time(&now);
    abstime.tv_sec= now + 5;
    abstime.tv_nsec = 0;
    pthread_mutex_lock( &condition_mutex ); //Wait until I get notified to try again
        pthread_cond_timedwait( &condition_cond, &condition_mutex, &abstime);
    pthread_mutex_unlock( &condition_mutex );

    printf("Ring");
}