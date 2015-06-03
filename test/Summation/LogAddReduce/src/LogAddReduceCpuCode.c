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
    const int outSize = 4;
    double *in        = malloc(sizeof(double)*inSize);
    double *dfeSums   = malloc(sizeof(double)*outSize);
    double *cpuSums   = malloc(sizeof(double)*outSize*2);

    double val[] = {1,2,7,4,5,11,3,9,1,4,5,2,1,8,4,0};
    // few test cases, different partitionings of given input sequence.
    // each number is repeated same amount of times as it says ---
    // that's poor man's way to tell the Kernel 'please reduce me
    // first 4 inputs, then next 8 inputs, then 1 input, then 3 last inputs';
    uint32_t nnz[10][16] = 
                     {
                      {4,4,4,4,8,8,8,8,8,8,8,8,1,3,3,3},
                      {3,3,3,5,5,5,5,5,6,6,6,6,6,6,2,2},
                      {1,2,2,3,3,3,10,10,10,10,10,10,10,10,10,10},
                      {7,7,7,7,7,7,7,5,5,5,5,5,1,3,3,3},
                      {9,9,9,9,9,9,9,9,9,4,4,4,4,2,2,1},
                      {11,11,11,11,11,11,11,11,11,11,11,2,2,2,2,1},
                      {12,12,12,12,12,12,12,12,12,12,12,12,2,2,1,1},
                      {13,13,13,13,13,13,13,13,13,13,13,13,13,1,1,1}
                     };

    printf("input sequence = {");
    for(int i = 0; i < inSize; ++i)
    {
        in[i]  = val[i];
        printf("%1.f ", val[i]);
    }
    printf("}\n");

    bool correct = true;
    for (int k = 0; k < 8; k++)
    {
        printf("\npartitioning # %d: {", k);
        for (int i = 0; i < outSize; ++i)
        {
            cpuSums[i] = 0.0;
        }
        int cnt = 0;
        int index = 0;
        for (int i = 0; i < outSize; ++i)
        {
            printf("%d, ", nnz[k][index]);
            for(int j = 0; j < nnz[k][index]; ++j)
            {
                cpuSums[i] += in[cnt++];
            }
            index += nnz[k][index];
        }

        printf("}\nRunning on DFE.");

        LogAddReduce(inSize, outSize, in, nnz[k], dfeSums);

        printf("\nDFE output: ");
        for(int i = 0; i < outSize; ++i)
        {
            printf(" %f", dfeSums[i]);
        }
        printf("\nCPU output: ");
        for(int i = 0; i < outSize; ++i)
        {
            printf(" %f", cpuSums[i]);
        }
        for(int i = 0; i < outSize; ++i)
        {
            if (cpuSums[i] - dfeSums[i] > 0.000001)
            {
                printf("\n error at position %d, cpu: %f, dfe: %f\n", i, cpuSums[i], dfeSums[i]);
                correct = false;
            }
        }
    }

    free(in);
    free(dfeSums);
    free(cpuSums);

    fflush(stdout);

    if (correct) printf("\n\nTest passed.\n\n");

    return !(correct == true);
}
