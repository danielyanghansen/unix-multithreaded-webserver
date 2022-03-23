#ifndef SEM_H
#define SEM_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/queue.h>
#include <errno.h>

#define NUM_THREADS 10

/*
 * Semaphore implementation for the synchronization of POSIX threads.
 *
 * This module implements counting P/V semaphores suitable for the 
 * synchronization of POSIX threads. POSIX mutexes and condition variables 
 * shall be utilized to implement the semaphore operations.
 */

/* Opaque type of a semaphore. 
 * ...you need to figure out the contents of struct SEM yourself!
 */
typedef struct SEM{
    int count;
    pthread_mutex_t condition_mutex;
    pthread_cond_t condition_cond;
    pthread_mutex_t count_mutex;
} SEM;

/* Creates a new semaphore.
 *
 * This function creates a new semaphore. If an error occurs during the 
 * initialization, the implementation shall free all resources already 
 * allocated so far.
 *
 * Parameters:
 *
 * @param initVal the initial value of the semaphore. This is the point where the SEM starts counting
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occured.
 */
SEM *sem_init(int initVal);

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * @param sem handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error. 
 *
 * In case of an error, not all resources may have been freed, but 
 * nevertheless the semaphore handle must not be used any more.
 */
int sem_del(SEM *sem);

/* P (wait) operation.
 * 
 * Attempts to decrement the semaphore value by 1. If the semaphore value 
 * is 0, the operation blocks until a V operation increments the value and 
 * the P operation succeeds.
 *
 * Parameters:
 *
 * @param sem handle of the semaphore to decrement
 */
void *P(SEM *sem);


/* V (signal) operation.
 *
 * Increments the semaphore value by 1 and notifies P operations that are 
 * blocked on the semaphore of the change.
 *
 * Parameters:
 *
 * @param sem handle of the semaphore to increment
 */
void *V(SEM *sem); 

#endif
