#include "sem.h"

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;


/*int sem_del(SEM *sem){
    if(pthread_attr_destroy(...)==0){
        return 0;
    }
    else {
        return -1;
    }
}*/

void V(SEM sem){
    for(;;)
    {
       pthread_mutex_lock( &condition_mutex );
       if( sem.count < COUNT_HALT1 || sem.count > COUNT_HALT2 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );

       pthread_mutex_lock( &count_mutex );
       sem.count++;
       printf("Counter value functionCount2: %d\n", sem.count);
       pthread_mutex_unlock( &count_mutex );

       if(sem.count >= COUNT_DONE) {return;
       }
    }
}

void P(SEM sem) {
   for(;;)
   {
      pthread_mutex_lock( &condition_mutex );
      while( sem.count >= COUNT_HALT1 && sem.count <= COUNT_HALT2 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
      sem.count++;
      printf("Counter value functionCount1: %d\n",sem.count);
      pthread_mutex_unlock( &count_mutex );

      if(sem.count >= COUNT_DONE){ 
          return;
      }
    }
}