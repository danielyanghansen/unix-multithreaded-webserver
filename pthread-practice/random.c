#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;


pthread_t threads[10];

void *funcP() {
    printf("FuncP\n");
    pthread_mutex_lock( &count_mutex );
    printf("Thread Number B: %ld\n", pthread_self());
    pthread_mutex_unlock( &count_mutex );
    
}

int main() {
    for (int i; i< 10; i++ ) {
        pthread_create( &threads[i], NULL, &funcP, NULL);
        printf("Thread Number A: %ld\n", threads[i]);
    }
}