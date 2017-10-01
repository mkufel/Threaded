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
    puts("");
}

static void *
do_flip(void * arg) {

    int buffer_index;
    int bit_index;

    int     parameter;
    parameter = * (int *) arg;              // get the integer value of the pointer
//    free (arg);             // we retrieved the integer value, so now the pointer can be deleted

    printf ("parameter=%d \n", parameter);

    for (int i = 2; i < 30; i++) {
        if(i % parameter == 0){

            buffer_index = i / 128;
            bit_index = i % 128;

            printf("Before: ");
            printBits(sizeof(buffer[buffer_index]), &buffer[buffer_index]);

            buffer[buffer_index] ^= (uint128_t) 1 << bit_index;

            printf("\n After: ");
            printBits(sizeof(buffer[buffer_index]), &buffer[buffer_index]);
            printf("\n");
        }
    }
    return NULL;
}

int main (void)
{

    pthread_t   my_threads[NROF_THREADS];   //array of thread id's

    int *       parameter;   // parameter to be handed over to the thread
    parameter = malloc (sizeof (int));  // memory will be freed by the child-thread
    *parameter = 2;        // assign an arbitrary value...


    for (int i = 1; i < NROF_THREADS; i++) {
        pthread_create(&my_threads[i], NULL, do_flip, parameter);
        pthread_join (my_threads[i], NULL);
        *parameter = 2 + i;
    }

    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)


    return (0);
}

