/* 
 * Operating Systems [2INCO] Practical Assignment
 * Threaded Application
 *
 * Ahmed Ahres (0978238)
 * Maciej Kufel (0944597)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * "Extra" steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uint128.h"
#include "flip.h"
#include <pthread.h>
#include <unistd.h>


// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t isThreadInitialized = PTHREAD_MUTEX_INITIALIZER;

static void *
do_flip(void * arg) {

    int buffer_index;
    int bit_index;
    int *   argi;
    int     param;

    argi = (int *) arg;     // proper casting before dereferencing (could also be done in one statement)
    param = *argi;          // get the integer value of the pointer
    pthread_mutex_unlock (&isThreadInitialized);

    for (int i = 2; i <= NROF_PIECES; i++) {

        if(i % param == 0)
        {
            buffer_index = i / 128; // The index of the buffer
            bit_index = i % 128; // The index of the bit in the buffer
            pthread_mutex_lock (&mutex); // Lock before entering critical section

            buffer[buffer_index] ^= (uint128_t) 1 << bit_index-1; // critical section

            pthread_mutex_unlock (&mutex); // Unlock after leaving the critical section
        }
    }
    return NULL;
}

int main (void)
{
    for (int l = 0; l < sizeof(buffer)/16; ++l) {
        buffer[l] = ~ 0; //Set all bits to 1
    }

    pthread_t   my_threads[NROF_THREADS];   //array of thread id's

    int *       parameter;   			// parameter to be handed over to the thread
    parameter = malloc (sizeof (int));  // memory will be freed by the child-thread
    *parameter = 1;        				// assign an arbitrary value...


    for (int i = 0; i < NROF_THREADS; i++) {
            pthread_mutex_lock (&isThreadInitialized);
            *parameter += 1;                                            //increase the parameter
            pthread_create(&my_threads[i], NULL, do_flip,
                           parameter);    //make a thread to the flipping with a certain parameter
    }

    for (int j = 0; j < sizeof(my_threads)/8; j++) 					//wait for all threads to terminate
    {
    	pthread_join (my_threads[j], NULL); // Wait for the threads
    }
    uint128_t mask = 1;

    for (int k = 0; k < sizeof(buffer)/16; k++) {
        for (int i = 0; i < 128; i++) {
            if (buffer[k]  & mask << i
            && ((k*128) + i) < NROF_PIECES)
            {
                printf("%d \n", (k*128) + i + 1); // Output the numbers
            }
        }
    }
    return (0);
}

