#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
    const int size = 4*100;
    float *in  = malloc(sizeof(float)*size);
    float *out = malloc(sizeof(float)*size);
    float *cpuSums = malloc(sizeof(float)*size);

    for(int i = 0; i < size; ++i) {
        in[i]  = (i+1)*i;
        out[i] = 0.0;
        cpuSums[i] = 2*in[i];
    }

    printf("Running on DFE.\n");
    NestedClass(size, in, out);

    printf("output from DFE:\n");
    for(int i = 0; i < 20; ++i)
    {
        printf(" %2.0f", out[i]);
    }
    printf("\nCPU calculated sums: \n");
    for(int i = 0; i < 20; ++i)
    {
        printf(" %2.0f", cpuSums[i]);
    }
    printf("\n");

    free(in);
    free(out);
    free(cpuSums);

    return 0; 
}
