#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 8;
    int sizeBytes = size * sizeof(int32_t);

    int32_t control[] = {2,6,1,7,4,2,4,6};

    int length = 0;
    for (int i = 0; i < size; i++)
    {
        length += control[i];
    }

    int32_t *innerCounter = malloc(length * sizeof(int32_t));
    int32_t *outerCounter = malloc(length * sizeof(int32_t));
    int32_t *delays       = malloc(length * sizeof(int32_t));


    printf("Running on DFE.\n");
    InputDependentCounter(size, length, control, innerCounter, outerCounter, delays);

    printf(" index:        ");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",i);
    }
    printf("\n inner counter:");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",outerCounter[i]);
    }
    printf("\n outer counter:");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",innerCounter[i]);
    }
    printf("\n delays:       ");
    for(int i = 0; i < length; ++i)
    {
        printf("%2d ",delays[i]);
    }
    free(innerCounter);
    free(outerCounter);
    free(delays);

    printf("Done.\n");
    return 0;
}
