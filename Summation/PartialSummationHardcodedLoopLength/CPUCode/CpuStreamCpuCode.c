#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 384;
    const int loopLength = 16;
    int sizeBytes = size * sizeof(float);
    float *in  = malloc(sizeBytes);
    float *out = malloc(sizeBytes);

    float sum = 0;
    for(int i = 0; i < size; ++i) {
        in[i]  = i+1;
        out[i] = 0.0;
    }

    for (int i = 0; i < size; i++)
    {
        sum += i+1;
    }

    printf("Running on DFE.\n");
    CpuStream(size, in, out);

    // TODO Use result data
    printf("output from DFE: kernel 1: ");
    float dfeSum = 0;
    for(int i = 0; i < loopLength; ++i)
    {
        dfeSum += out[i];
        printf(" %f", out[i]);
    }
    printf("\nSum of values from DFE = %f", dfeSum);
    printf("\nCPU calculated sum     = %f\nDone.\n",sum);
    free(in);
    free(out);

    return !(dfeSum == sum);
}
