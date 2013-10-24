/*

 This project complements maxcompiler tutorial on advanced counters.
 Here, the kernel maintains two counters representing nested loop
 where number of iterations in the inner loop changes on each iteration
 of an outer loop.

 Tested in simulation mode, has NOT been tested on hardware.

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

    const int inSize = 32;

    int32_t in[] = {2,2,
                    1,
                    3,3,3,
                    4,4,4,4,
                    5,5,5,5,5,
                    3,3,3,
                    7,7,7,7,7,7,7,
                    5,5,5,5,5,
                    2,2};
    int32_t *outer     = malloc(sizeof(int32_t)*inSize);
    int32_t *nested    = malloc(sizeof(int32_t)*inSize);

    printf("Running on DFE.\n");
    NestedCounterVariableBoundary(inSize, in, nested, outer);

    printf("input to DFE:                    ");
    for(int i = 0; i < inSize; ++i)
    {
        printf(" %d", in[i]);
    }
    printf("\noutput from DFE: outer counter:  ");
    for(int i = 0; i < inSize; ++i)
    {
        printf(" %d", outer[i]);
    }
    printf("\noutput from DFE: nested counter: ");
    for(int i = 0; i < inSize; ++i)
    {
        printf(" %d", nested[i]);
    }
    printf("\n");
    free(outer);
    free(nested);

    return 0;
}
