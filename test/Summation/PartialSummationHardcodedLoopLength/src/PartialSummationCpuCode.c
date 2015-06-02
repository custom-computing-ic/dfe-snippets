/*

 This project implements partial summation on DFE with fixed length of
 feedback summation loop. The length of this loop is hardcoded so that it
 exceeds the summator latency (important for floating point arithmetic
 adder which has latency 13, for FPGAs used in MAX4). This is
 probably the simpliest way of implementing summation (or whatever
 computation with accumulated state) with minimal hardware costs.

 Tested in simulation mode, has NOT been tested on hardware.

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 384;
    float *in  = malloc(sizeof(float)*size);
    // this constant is defined in Maxfiles.h
    float *out = malloc(sizeof(float)*size);//PartialSummation_sumLoopLength);

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
    PartialSummation(size, in, out);

    // TODO Use result data
    printf("output from DFE: kernel 1: ");
    float dfeSum = 0;
    for(int i = 0; i < PartialSummation_sumLoopLength; ++i)
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
