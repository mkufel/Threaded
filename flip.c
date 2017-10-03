/* 
 * Operating Systems [2INCO] Practical Assignment
 * Threaded Application
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
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

// create a bitmask where bit at position n is set
#define BITMASK          ((uint128_t) 0xffffffffffffffff)

// check if bit n in v is set
#define BIT_IS_SET(v,n)     (((v) & BITMASK(n)) == BITMASK(n))

static pthread_mutex_t      mutex          = PTHREAD_MUTEX_INITIALIZER;

static bool * isThreadInitialized = true;

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("\n");
}

static void *
do_flip(void * arg) {

    int buffer_index;
    int bit_index;

    int *   argi;
    int     param;

    argi = (int *) arg;     // proper casting before dereferencing (could also be done in one statement)
    param = *argi;              // get the integer value of the pointer
//    printf("Pointer to arg: %p, pointer to argi: %p, pointer to i %p\n", arg, argi, (&param));
    isThreadInitialized = true;

    for (int i = 2; i < NROF_PIECES; i++) {

        if(i % param == 0)
        {
            buffer_index = i / 128;
            bit_index = i % 128;

            pthread_mutex_lock (&mutex);

//            printf("Before with param: %d and a piece: %d\n", param, i);
//            printBits(sizeof(buffer[buffer_index]), &buffer[buffer_index]);

            buffer[buffer_index] ^= (uint128_t) 1 << bit_index-1;

//            printf("\n After: ");
//            printBits(sizeof(buffer[buffer_index]), &buffer[buffer_index]);
//            printf("\n");

            pthread_mutex_unlock (&mutex);
        }
    }
    return NULL;
}

int main (void)
{

    pthread_t   my_threads[NROF_THREADS];   //array of thread id's

    int *       parameter;   			// parameter to be handed over to the thread
    parameter = malloc (sizeof (int));  // memory will be freed by the child-thread
    *parameter = 1;        				// assign an arbitrary value...


    for (int i = 0; i < NROF_THREADS; i++) {

        if (isThreadInitialized)
        {
            *parameter += 1;                                            //increase the parameter
            pthread_create(&my_threads[i], NULL, do_flip,
                           parameter);    //make a thread to the flipping with a certain parameter
            isThreadInitialized = false;
        }
        else if (i < NROF_THREADS)
        {

            i--;
        }
        else break;
    }

    for (int j = 0; j < sizeof(my_threads)/8; j++) 					//wait for all threads to terminate
    {
    	pthread_join (my_threads[j], NULL);
    }

    printBits(sizeof(uint128_t), &buffer[0]);
    uint128_t mask = 1;

    printBits(sizeof(uint128_t), &mask);

    for (int k = 0; k < sizeof(buffer)/sizeof(uint16_t); k++) {
        for (int i = 0; i < sizeof(uint128_t); i++) {
            if (buffer[k]  & mask << i
            && ((k*128) + i) < NROF_PIECES)
            {
                printf("%d, ", (k*128) + i);
            }
        }
    }

    printf("\n");

    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    ////  see bit_test() how to manipulate bits in a large integer)


    return (0);
}

