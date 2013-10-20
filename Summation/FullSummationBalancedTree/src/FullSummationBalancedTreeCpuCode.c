#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int inSize = 384;
    const int loopLength = 16;
    const int minimalPCIeStreamLength = 4;
    float *in       = malloc(sizeof(float)*inSize);
    float *full     = malloc(sizeof(float)*minimalPCIeStreamLength);
    float *partial  = malloc(sizeof(float)*loopLength);

    float sum = 0;
    for(int i = 0; i < inSize; ++i) {
        in[i]  = i+1;
        sum   += i+1;
    }

    printf("Running on DFE.\n");
    FullSummationBalancedTree(inSize, in, full, partial);

    printf("output from DFE, full summation (stream): ");
    float dfeTotalSum = full[minimalPCIeStreamLength-1];
    float dfeSum = 0;
    for(int i = 0; i < minimalPCIeStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\noutput from DFE: partial sums: ");
    for(int i = 0; i < loopLength; ++i)
    {
        dfeSum += partial[i];
        printf(" %f", partial[i]);
    }
    printf("\nTotal sum from DFE             = %f", dfeTotalSum);
    printf("\nSum of partial values from DFE = %f", dfeSum);
    printf("\nCPU calculated sum             = %f\nDone.\n",sum);
    free(in);
    free(partial);
    free(full);

    return !((dfeSum == sum) && (dfeSum == dfeTotalSum));
}
