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
#include "uint128.h"
#include "flip.h"
#include <pthread.h>


static pthread_mutex_t threadInitMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexes[sizeof(buffer)/sizeof(uint128_t )];
static uint128_t mask = 1; // bit mask used to check the result, initialize with LSB = 1



void initialize(void)   //Initialize mutexes and set the buffer bits to 1
{
    for (int i = 0; i < sizeof(buffer)/sizeof(uint128_t ); i++) {

        pthread_mutex_init(&mutexes[i], NULL);
        buffer[i] = ~ 0; //Set all bits to 1
    }
}


void create_and_execute_threads(void)
{
    pthread_t   my_threads[NROF_THREADS];   //array of thread id's

    int *       parameter;   			// parameter to be handed over to the thread
    parameter = malloc (sizeof (int));  // memory will be freed by the child-thread
    *parameter = 1;        				// assign an arbitrary value


    for (int i = 0; i < NROF_THREADS; i++) {            //create threads
        //lock the threadInitMutex, enter iff previously created thread already dereferenced the parameter pointer)
        pthread_mutex_lock (&threadInitMutex);

        * parameter += 1;                               //increase the parameter
        pthread_create(&my_threads[i], NULL, do_flip,
                       parameter);                      //make a thread to the flipping with a certain parameter
    }

    for (int j = 0; j < sizeof(my_threads)/sizeof(uint64_t); j++) 					//wait for all threads to terminate
    {
        pthread_join (my_threads[j], NULL); // Wait for every single thread in the array
    }
}


static void *
do_flip(void * arg) {   //thread job, flip multiples of the passed parameter

    int buffer_index;
    int bit_index;
    int     param;

    param = * (int *) arg;     // casting and dereferencing the passed argument pointer
    pthread_mutex_unlock (&threadInitMutex); // value of the param 'retrieved', unlock the threadInitMutex

    for (int i = 2; i <= NROF_PIECES; i++) {

        if(i % param == 0)
        {
            buffer_index = i / 128; // The index of the buffer
            bit_index = i % 128; // The index of the bit in the buffer

            pthread_mutex_lock (&mutexes[buffer_index]); // Lock the mutex of the buffer you want to flip

            buffer[buffer_index] ^= (uint128_t) 1 << bit_index-1; // critical section, flip the bit

            pthread_mutex_unlock (&mutexes[buffer_index]); // Unlock after leaving the critical section
        }
    }

    return NULL;
}


void print_output(void)
{
    for (int k = 0; k < sizeof(buffer)/sizeof(uint128_t ); k++) {
        for (int i = 0; i < 128; i++) {

            //if buffer element LAND mask with the 1 shifted i-places to the left and the bit within the result
            if ( (buffer[k] & mask << i) && ( ( 128*k + i ) < NROF_PIECES) )
            {
                printf("%d \n", (k*128) + i + 1); // Output the numbers, kth buffer element (i+1)th bit
            }
        }
    }
}


int main (void)
{
    initialize();
    create_and_execute_threads();
    print_output();

    return (0);
}