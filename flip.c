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

int main (void)
{

    int buffer_index;
    int bit_index;
    int currentMultiple = 2;

    printf("\n %x \n", 60);

    for (int i = 2; i < 30; i++) {
        if(i % currentMultiple == 0){
            buffer_index = i / 128;
            bit_index = i % 128;
            printf("Current i: %x. Initial: %llx \n", i, (int) buffer[buffer_index]);
            buffer[buffer_index] = buffer[buffer_index] ^ (1 << bit_index);
            printf("Flipped: %x\n \n", (int) buffer[buffer_index]);
        }
    }

    // TODO: start threads to flip the pieces and output the results
    // (see thread_test() and thread_mutex_test() how to use threads and mutexes,
    //  see bit_test() how to manipulate bits in a large integer)


    return (0);
}

