/***

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

    const int inSize  = 16;
    double *in        = malloc(sizeof(double)*inSize);
    double *dfeSums   = malloc(sizeof(double)*inSize);
    double *cpuSums   = malloc(sizeof(double)*inSize*2);

    double val[] = {1,2,7,4,5,11,3,9,1,4,5,2,1,8,4,0};

    printf("input sequence = {");
    for(int i = 0; i < inSize; ++i)
    {
        in[i]  = val[i];
        printf("%1.f ", val[i]);
    }
    printf("}\n");

    for(int i = 0; i < inSize; ++i)
    {
        long factor = 1;
        long power = 1;
        for (int j = 1; j < ReduceStaggeredVectors_numVectors; j++)
        {
            power *= 2;
            factor += power;
        }
        cpuSums[i] = factor*in[i];
    }

    printf("\nRunning on DFE.\n\n");

    ReduceStaggeredVectors(inSize, in, dfeSums);

    printf("\nDFE output: ");
    for(int i = 0; i < inSize; ++i)
    {
        printf(" %f", dfeSums[i]);
    }
    printf("\nCPU output: ");
    for(int i = 0; i < inSize; ++i)
    {
        printf(" %f", cpuSums[i]);
    }

    bool correct = true;
    for(int i = 0; i < inSize; ++i)
    {
        if (cpuSums[i] - dfeSums[i] > 0.000001)
        {
            printf("\n error at position %d, cpu: %f, dfe: %f\n", i, cpuSums[i], dfeSums[i]);
            correct = false;
        }
    }

    free(in);
    free(dfeSums);
    free(cpuSums);

    fflush(stdout);

    if (correct) printf("\n\nTest passed.\n\n");

    return !(correct == true);
}
