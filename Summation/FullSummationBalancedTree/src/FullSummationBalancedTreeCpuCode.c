/*

 This project implements full summation on DFE with fixed length of
 feedback summation loop. The length of this loop is hardcoded so that
 it exceeds the summator latency (important for floating point
 arithmetic adder which has latency 13, for FPGAs used in MAX4).

 Full summation is achieved by summation of all output values using
 binary tree of stream offsets.

 Tested in simulation mode, has NOT been tested on hardware.

*/
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
    float *full      = malloc(sizeof(float)*FullSummationBalancedTree_minimalPciStreamLength);
    float *partial   = malloc(sizeof(float)*FullSummationBalancedTree_sumLoopLength);

    float sum = 0;
    for(int i = 0; i < inSize; ++i) {
        in[i]  = i+1;
        sum   += i+1;
    }

    printf("Running on DFE.\n");
    FullSummationBalancedTree(inSize, in, full, partial);

    printf("output from DFE, full summation (stream): ");
    float dfeTotalSum = full[FullSummationBalancedTree_minimalPciStreamLength-1];
    float dfeSum = 0;
    for(int i = 0; i < FullSummationBalancedTree_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\noutput from DFE: partial sums: ");
    for(int i = 0; i < FullSummationBalancedTree_sumLoopLength; ++i)
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
