#include "sem.h"
#include "common.h"

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

int int_thread = NUM_THREADS;
int num_threads = NUM_THREADS;
int threads[NUM_THREADS];

int next_thread() {
    if (int_thread >= num_threads - 1 ) {
        int_thread = 0;
    } else {
        int_thread++;
    }
    return int_thread;
}

/**
 * @brief Is this necessary?
 * 
 */
void mem_alloc() {
    int* threads = (int *) malloc(10 * sizeof(int));
}

SEM *sem_init(int initVal){
    Queue *q = ConstructQueue(NUM_THREADS);
    SEM *sem = (SEM *){initVal, q};
  /*  pthread_t newthread;
    pthread_t newthread2;

    if (!(threads[NUM_THREADS])) {
        threads[NUM_THREADS];
    }

    //Thread one 
    int one = next_thread();
    int P_thread = pthread_create(&newthread, NULL, P(sem), NULL); 
    threads[int_thread] = P_thread;
    pthread_join(threads[one], NULL);
    //Thread two
    int two = next_thread();
    int V_thread = pthread_create(&newthread2, NULL, V(sem), NULL); 
    threads[int_thread] = V_thread;
    pthread_join(threads[two], NULL);

    //Attach/join threads
*/
    return sem; 
}

/*
int sem_del(SEM *sem){
    if(pthread_attr_destroy(SEM *sem)==0){
        return 0;
    }
    else {
        return -1;
    }
}
*/

void *V(SEM sem){
    while(1)
    {
       pthread_mutex_lock( &condition_mutex );
       if( sem.count < COUNT_HALT1 || sem.count > COUNT_HALT2 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );

       pthread_mutex_lock( &count_mutex );
       sem.count++;

       pthread_mutex_unlock( &count_mutex );

       if(sem.count >= COUNT_DONE) {
           return;
       }
    }
}

void *P(SEM sem) {
   while(1)
   {
      pthread_mutex_lock( &condition_mutex );
      while( sem.count >= COUNT_HALT1 && sem.count <= COUNT_HALT2 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
      sem.count++;

      pthread_mutex_unlock( &count_mutex );

      if(sem.count >= COUNT_DONE){ 
          return;
      }
    }
    //funksjonene bestemmer når på counten de kjører men det kan ikke være konstanter ettersom vi trenger å bruke de for flere
    //trenger også en måte å få størrelsen slik at vi kan vite hvilke størrelse listen skal være men 
}