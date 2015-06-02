#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 16;
    int sizeBytes = size * sizeof(int32_t);
    int32_t in[] = {10,11,30,13,14,15,16,17,18,19,20,21,22,23,24,25};
    int32_t *out = malloc(size * sizeof(int32_t));

    printf("Running on DFE.\n");
    CircularBuffer(size, in, out);

    printf("\ncycle: ");
    for(int i = 0; i < size; ++i)
    {
        printf("%2d ",i);
    }
    printf("\nin:    ");
    for(int i = 0; i < size; ++i)
    {
        printf("%2d ",in[i]);
    }
    printf("\nout:   ");
    for(int i = 0; i < size; ++i)
    {
        printf("%2d ",out[i]);
    }

    free(out);

    printf("Done.\n");
    return 0;
}
