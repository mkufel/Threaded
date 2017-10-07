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
#include <pthread.h>
#include "flip.h"


static pthread_mutex_t thread_init_mutex = PTHREAD_MUTEX_INITIALIZER; //used to retrieve a param of a created thread
static pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;  //used to modify running_threads
static pthread_mutex_t mutexes[sizeof(buffer)/sizeof(uint128_t )]; // array of access mutexes for every buffer index
static pthread_cond_t thread_finished = PTHREAD_COND_INITIALIZER;
static pthread_cond_t all_threads_finished = PTHREAD_COND_INITIALIZER;

static int running_threads = 0; //number of currently running children threads


static void *
do_flip(void * arg) {   //thread job, flip multiples of the passed parameter

    int buffer_index;
    int bit_index;
    int     param;

    param = * (int *) arg;     // casting and dereferencing the passed argument pointer

    if (pthread_mutex_unlock (&thread_init_mutex) != 0) { // value of the param retrieved, unlock the threadInitMutex
        perror("error while trying to unlock the thread_init_mutex in a thread");
    }

    for (int i = 2; i <= NROF_PIECES; i++) {   //flip multiples of the given parameter

        if(i % param == 0)
        {
            buffer_index = (i-1) / 128; // The index of the buffer
            bit_index = (i-1) % 128; // The index of the bit in the buffer

            if (pthread_mutex_lock (&mutexes[buffer_index]) != 0) { // Lock the mutex of the buffer you want to flip
                perror("error while trying to lock the mutex for an element in the buffer");
            }

            buffer[buffer_index] ^= (uint128_t) 1 << bit_index; // critical section, flip the bit

            if (pthread_mutex_unlock (&mutexes[buffer_index]) != 0) { // Unlock after leaving the critical section
                perror("error while trying to unlock the mutex for an element in the buffer");
            }
        }
    }

    if (pthread_mutex_lock(&thread_lock) != 0) {
        perror("error while trying to lock the mutex for running threads in a thread");
    }

    running_threads--;

    if (running_threads == 0)
    {
        if (pthread_cond_signal(&all_threads_finished) != 0) {
            perror("error while trying to signal all_threads_finished");
        }
    }

    if (pthread_cond_signal(&thread_finished) != 0) {
        perror("error while trying to signal thread_finished");
    }

    if (pthread_mutex_unlock(&thread_lock) != 0) {
        perror("error while trying to unlock the mutex for running threads in a thread");
    }

    return NULL;
}


void initialize(void)   //Initialize mutexes and set the buffer bits to 1
{
    for (int i = 0; i < sizeof(buffer)/sizeof(uint128_t ); i++) {

        pthread_mutex_init(&mutexes[i], NULL);
        buffer[i] = ~ 0; //Set all bits to 1
    }
}


void create_and_execute_threads(void)
{
    pthread_t   my_threads[NROF_PIECES-1];   //array of thread id's (excluded 1)

    int *       parameter;   			// parameter to be handed over to the thread
    parameter = malloc (sizeof (int));  // memory will be freed by the child-thread
    *parameter = 1;        				// assign an arbitrary value

    for (int i = 2; i <= NROF_PIECES; i++) {

        if (pthread_mutex_lock(&thread_lock) != 0) {
            perror("error while trying to lock the mutex for running threads");
        }

        if (running_threads == NROF_THREADS)    //if the max amount of threads running, wait for thread_finished
        {
            if (pthread_cond_wait(&thread_finished, &thread_lock) != 0) {
                perror("error while waiting for thread_finished");
            }
        }

        if (pthread_mutex_lock (&thread_init_mutex) != 0) { //lock before modifying the parameter
            perror("error while trying to lock the thread_init_mutex in a thread");
        }

        * parameter += 1;                               //increase the parameter

        //create a thread to flip multiples of the parameter
        if( pthread_create(&my_threads[i-2], NULL, do_flip, parameter) != 0) {
            perror("error while creating a thread");
        }

        running_threads++;

        if (pthread_mutex_unlock(&thread_lock) != 0) {
            perror("error while trying to unlock the mutex for running threads");
        }
    }


    while (running_threads > 0) {  //if there are still threads running, wait for the notification that all terminated

        if (pthread_cond_wait(&all_threads_finished, &thread_lock) != 0) {
            perror("error while waiting for all_threads_finished");
        }
    }
}




void print_output(void)
{
    uint128_t mask = 1; // bit mask used to check the result, initialize with LSB = 1

    for (int k = 0; k < sizeof(buffer)/sizeof(uint128_t ); k++) {
        for (int i = 0; i < 128; i++) {

            //if buffer element LAND mask with the 1 shifted i-places to the left and the bit within the result
            if ( (buffer[k] & mask << i) && ( ( 128*k + i ) < NROF_PIECES) )
            {
                printf("%d\n", (k*128) + i + 1); // Output the numbers, kth buffer element (i+1)th bit
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