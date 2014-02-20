/***

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

    const int inSize  = 8; //24;
    double *in        = malloc(sizeof(double)*inSize);

    // these two constants are defined in Maxfiles.h
    double *full      = malloc(sizeof(double)*FullSummationLogarithmicCost_minimalPciStreamLength);
    double *partial   = malloc(sizeof(double)*inSize);

    // showing that scheme works for 24 values, 20 of which represent padding.
    // (i.e. it works for accumulating 4 values).
    double sum = 0;
    for(int i = 0; i < inSize; ++i)
    {
        in[i]  = i+1;
    }
    for(int i = 0; i < 4; ++i)
    {
        in[i]  = 0;
    }
    for(int i = 0; i < inSize; ++i)
    {
        sum   += in[i];
    }

    printf("Running on DFE.\n");
    FullSummationLogarithmicCost(inSize, in, full,partial);

    printf("output from DFE, full summation (stream): ");
    double dfeTotalSum = full[0];
    double dfeSum = 0;
    for(int i = 0; i < FullSummationLogarithmicCost_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\noutput from DFE: partial sums: ");
    for(int i = inSize-FullSummationLogarithmicCost_numPartialSums; i < inSize; ++i)
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
