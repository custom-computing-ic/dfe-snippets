#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 12;
    int sizeBytes = size * sizeof(int32_t);

    int32_t control[] = {0,1,8,15,19,21,25,32,35,37,40,48};

    int length = control[size-1];

    int32_t *innerCounter = malloc(length * sizeof(int32_t));
    int32_t *outerCounter = malloc(length * sizeof(int32_t));
    int32_t *delays       = malloc(length * sizeof(int32_t));
    int32_t *sparseIndex  = malloc(length * sizeof(int32_t));


    printf("Running on DFE.\n");
    InputDependentCounterIterateUntil(size, length, control, delays, innerCounter, outerCounter, sparseIndex);

    printf(" cycle:        ");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",i);
    }
    printf("\n sparseIndex:  ");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",sparseIndex[i]);
    }
    printf("\n count times:  ");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",delays[i]);
    }
    printf("\n inner counter:");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",innerCounter[i]);
    }
    printf("\n outer counter:");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",outerCounter[i]);
    }

    free(innerCounter);
    free(outerCounter);
    free(delays);
    free(sparseIndex);

    printf("\nDone.\n");
    return 0;
}
