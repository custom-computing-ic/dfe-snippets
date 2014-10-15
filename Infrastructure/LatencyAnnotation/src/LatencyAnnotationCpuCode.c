#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

    const int inSize = 384;
    float *in        = malloc(sizeof(float)*inSize);

    // these two constants are defined in Maxfiles.h
    float *full      = malloc(sizeof(float)*LatencyAnnotation_minimalPciStreamLength);
    float *partial   = malloc(sizeof(float)*LatencyAnnotation_sumLoopLength);

    float sum = 0;
    for(int i = 0; i < inSize; ++i) {
        in[i]  = i+1;
        sum   += i+1;
    }

    printf("Running on DFE.\n");
    LatencyAnnotation(inSize, in, full, partial);

    printf("output from DFE, full summation (stream): ");
    float dfeTotalSum = full[LatencyAnnotation_minimalPciStreamLength-1];
    float dfeSum = 0;
    for(int i = 0; i < LatencyAnnotation_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\noutput from DFE: partial sums: ");
    for(int i = 0; i < LatencyAnnotation_sumLoopLength; ++i)
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
