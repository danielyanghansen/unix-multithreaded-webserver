
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

void *funcP(); //P
void *funcV(); //V
int  count = 0;
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6
int SEMish;

pthread_t manager;
pthread_t threads[10];

main()
{
   for (int i=0; i<10; i+=1) {
       pthread_create( &threads[i], NULL, &funcP, NULL);
       printf("Thread %d: %ld \n", i, threads[i]);
   }
   pthread_create(&manager, NULL, &funcV, NULL);
   printf("Manager thread: %ld\n", manager);
   for (int i=0; i<10; i++) {
       pthread_join(threads[i], NULL);
   } //joining means "get ready for public listening"
   pthread_join(manager, NULL);
   SEMish = 0;

   exit(0);
}

void *funcP()
{
    for(;;)
    {
        if(count >= COUNT_DONE) return(NULL);
        while( count <= 1 ) {
            if (threads[SEMish] == pthread_self()) {
                pthread_mutex_lock( &condition_mutex );
                pthread_cond_wait( &condition_cond, &condition_mutex );
                pthread_mutex_unlock( &condition_mutex );
            }
        }   
        printf("Thread Number: %ld\n", pthread_self());
        printf("SEMish Thread %ld\n", threads[SEMish]);

        pthread_mutex_lock( &count_mutex );
            count++;
            SEMish++;
            if (SEMish >= 10) {SEMish = 0;}
            printf("Counter value funcP: %d\n\n",count);
        pthread_mutex_unlock( &count_mutex );

    }
}

void *funcV()
{
    while(1)
    {
        if(count >= COUNT_DONE) return(NULL);

        if( count > 1 )
        {//pthread cond signal unlocker minst en av de threadsene som er blocked(som wait) og har condition_cond
            pthread_mutex_lock( &condition_mutex );
            pthread_cond_signal( &condition_cond );
            pthread_mutex_unlock( &condition_mutex );
        }
        printf("Thread Number: %ld\n", pthread_self());
        printf("SEMish Thread %ld\n", threads[SEMish]);

        pthread_mutex_lock( &count_mutex );
            count++;
            SEMish++;
            if (SEMish >= 10) {SEMish = 0;}
            printf("Counter value funcV: %d\n\n",count);
        pthread_mutex_unlock( &count_mutex );

    }

}