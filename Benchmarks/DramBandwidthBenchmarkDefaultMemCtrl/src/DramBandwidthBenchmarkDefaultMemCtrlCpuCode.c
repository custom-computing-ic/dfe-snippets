/*
    
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "Maxfiles.h"
#include "MaxSLiCInterface.h"


void iffree(void * p)
{
    if (p != NULL) free(p);
}

int main(void)
{
    const int repeatTimes = 50;
    const int vectorSize = 384*1000000;

    const double datasetSizeMiB = vectorSize*sizeof(float)/(1024*1024.0);

    printf("\nvector size = %d\n",vectorSize);
    printf("data set size %f MiB\n", datasetSizeMiB );

    float *vector = malloc(sizeof(float)*vectorSize);
    float *outstream_sum = malloc(sizeof(float)*DramBandwidthBenchmarkDefaultMemCtrl_minimalPciStreamLength);

    for(int i = 0; i < vectorSize; ++i)
    {
        float x = (i + 1) % 4;
        vector[i] = x*x;
    }

    printf("timing CPU implementation:\n");

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
        float sum = 0;
        for(int j = 0; j < repeatTimes; ++j)
        {
            for(int i = 0; i < vectorSize; ++i)
            {
                const float v = vector[i] + 1;
                sum += v;
            }
        }
    gettimeofday(&t2, NULL);

    printf("ignore this number: %f\n", sum);

    double cpu_microSeconds = (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + (t2.tv_usec - t1.tv_usec);
    double cpu_seconds = cpu_microSeconds/(1000.0*1000.0);
    double cpu_bandwidth = (datasetSizeMiB*repeatTimes)/cpu_seconds;

    printf("\nDone on CPU in %.2f seconds (%.2f microsec), bandwidth %.2f MiB/s \n", cpu_seconds, cpu_microSeconds, cpu_bandwidth);
    printf("\n");

    // write vector data to DRAM
    DramBandwidthBenchmarkDefaultMemCtrl_WriteLMem(vectorSize, vector);

    printf("timing DFE implementation:");

    gettimeofday(&t1, NULL);
        for(int j = 0; j < repeatTimes; ++j)
        {
            DramBandwidthBenchmarkDefaultMemCtrl(vectorSize, outstream_sum);
        }
    gettimeofday(&t2, NULL);

    double dfe_microSeconds = (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + (t2.tv_usec - t1.tv_usec);
    double dfe_seconds = dfe_microSeconds/(1000.0*1000.0);
    double dfe_bandwidth = (datasetSizeMiB*repeatTimes)/dfe_seconds;

    printf("\nDone on DFE in %.2f seconds (%.2f microsec), bandwidth %.2f MiB/s \n", dfe_seconds, dfe_microSeconds, dfe_bandwidth);


    iffree(outstream_sum);
    iffree(vector);

    return 0;
}
