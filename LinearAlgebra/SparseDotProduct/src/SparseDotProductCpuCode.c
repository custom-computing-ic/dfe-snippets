/*

 This project demonstrates performing dot product computation
 with sparse vector (fully fits to on-chip BRAM) and sparse
 index streamed via PCIe.

 Tested in simulation mode, has NOT been tested on hardware.

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int dataSize=32;
    const int inSize = 16;

    uint8_t *in      = malloc(sizeof(uint8_t)*inSize);
    double *vector   = malloc(sizeof(double)*dataSize);

    // this constant is defined in Maxfiles.h
    float *full      = malloc(sizeof(float)*SparseDotProduct_minimalPciStreamLength);

    float sum = 0;
    for(int i = 0; i < dataSize; ++i) {
        vector[i]  = i+1;
    }
    for(int i = 0; i < inSize; ++i) {
        in[i]  = 2*i;
        sum   += vector[in[i]]*vector[in[i]];
    }

    printf("Running on DFE.\n");
    SparseDotProduct(inSize, in, full, vector);

    printf("output from DFE, full summation (stream): ");
    float dfeTotalSum = full[SparseDotProduct_minimalPciStreamLength-1];
    float dfeSum = 0;
    for(int i = 0; i < SparseDotProduct_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\nTotal sum from DFE             = %f", dfeTotalSum);
    printf("\nCPU calculated sum             = %f\nDone.\n",sum);

    printf("\n----Updating index, keeping vector the same----\n\n");

    sum = 0.0;
    for(int i = 0; i < inSize; ++i) {
        in[i]  = 2*i+1;
        sum   += vector[in[i]]*vector[in[i]];
    }

    printf("Running on DFE.\n");
    SparseDotProduct(inSize, in, full, vector);

    printf("output from DFE, full summation (stream): ");
    dfeTotalSum = full[SparseDotProduct_minimalPciStreamLength-1];
    dfeSum = 0;
    for(int i = 0; i < SparseDotProduct_minimalPciStreamLength; ++i)
    {
        printf(" %f", full[i]);
    }
    printf("\nTotal sum from DFE             = %f", dfeTotalSum);
    printf("\nCPU calculated sum             = %f\nDone.\n",sum);



    free(in);
    free(vector);
    free(full);

    return !(sum == dfeTotalSum);
}
