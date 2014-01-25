/*

 This project implements full summation on DFE with fixed length of
 feedback summation loop. The length of this loop is hardcoded so that it
 exceeds the summator latency (important for floating point arithmetic
 adder which has latency 13, for FPGAs used in MAX4). Full summation
 is achieved by summation of all output values using stream offsets.

 This is probably an implementation with maximal hardware costs.

 Tested in simulation mode, has NOT been tested on hardware.

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

    const int inSize = 12;
    float *in        = malloc(sizeof(float)*inSize);

    // these two constants are defined in Maxfiles.h
    float *full      = malloc(sizeof(float)*FullSummationLogarithmicCost_minimalPciStreamLength);
    float *partial   = malloc(sizeof(float)*inSize);//FullSummationLogarithmicCost_schemeLatency);

    float sum = 0;
    for(int i = 0; i < inSize; ++i) {
        in[i]  = i+1;
        sum   += i+1;
    }

    printf("Running on DFE.\n");
    FullSummationLogarithmicCost(inSize, in, partial);//full, 

    printf("output from DFE, full summation (stream): ");
    float dfeTotalSum = full[FullSummationLogarithmicCost_minimalPciStreamLength-1];
    float dfeSum = 0;
    for(int i = 0; i < FullSummationLogarithmicCost_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\noutput from DFE: partial sums: ");
    for(int i = 0; i < inSize; ++i)//FullSummationLogarithmicCost_schemeLatency
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
