#ifndef ____BBUFFER___H___
#define ____BBUFFER___H___
#include <pthread.h>

#define BUFSIZ 256

/*
 * Bounded Buffer implementation to manage int values that supports multiple 
 * readers and writers.
 *
 * The bbuffer module uses the sem module API to synchronize concurrent access 
 * of readers and writers to the bounded buffer.
 */

/* Opaque type of a Bounded Buffer.
 * ...you need to figure out the contents of struct BNDBUF yourself
 */
typedef struct BNDBUF { //Remember mutual exclusion during 1: Updating next_in and 2: Updating next_out
    pthread_mutex_t buf_mutex;
    int bufsiz;
    int count;
    int next_in; 
    int next_out;
    
    int value[1];  //VERY IMPORTANT FOR THIS TO BE AT THE END, WHICH ALLOWS MALLOC TO GO OUT OF BOUNDS
} BNDBUF;   

/* Creates a new Bounded Buffer. 
 *
 * This function creates a new bounded buffer and all the helper data 
 * structures required by the buffer, including semaphores for 
 * synchronization. If an error occurs during the initialization the 
 * implementation shall free all resources already allocated by then.
 *
 * 
 * Parameters:
 *
 * @param size The number of integers that can be stored in the bounded buffer.
 *
 * Returns:
 *
 * @returns bb - handle for the created bounded buffer, or NULL if an error occured.
 */
BNDBUF *bb_init(unsigned int size);

/* Destroys a Bounded Buffer. 
 *
 * All resources associated with the bounded buffer are released.
 *
 * Parameters:
 *
 * @param bb Handle of the bounded buffer that shall be freed.
 */
void bb_del(BNDBUF *bb);

/* Retrieve an element from the bounded buffer. (Consumer consumes item)
 *
 * This function removes an element from the bounded buffer. If the bounded 
 * buffer is empty, the function blocks until an element is added to the 
 * buffer.
 *
 * Parameters:
 *
 * @param bb Handle of the bounded buffer.
 *
 * Returns:
 *
 * @returns the int element, or -1 if there was an error
 */
int  bb_get(BNDBUF *bb);

/* Add an element to the bounded buffer. (Producer produces item)
 *
 * This function adds an element to the bounded buffer. If the bounded 
 * buffer is full, the function blocks until an element is removed from 
 * the buffer.
 *
 * Parameters:
 *
 * @param bb Handle of the bounded buffer.
 * @param fd Value that shall be added to the buffer.
 *
 * Returns:
 *
 * @returns the int element, or -1 of there was an error
 */
int bb_add(BNDBUF *bb, int fd);

#endif