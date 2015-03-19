/*
    
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


#include "Maxfiles.h"
#include "MaxSLiCInterface.h"


void iffree(void * p)
{
    if (p != NULL) free(p);
}

int main(void)
{
    const int repeatTimes = 50;

    // size in bytes: 24GB, split into 2 streams
    const uint64_t vectorSize = 24*(uint64_t)1024*1024*1024;
    const double datasetGB = vectorSize/(1024.0*1024.0*1024.0);
    // how many ticks the kernel should run: kernel reads 384 bytes per cycle
    const uint64_t ticksToRun = vectorSize/384;

    printf("\nvector size = %ld\n",vectorSize);
    printf("data set size %f GB\n", datasetGB );

    uint64_t *outstream_sum = malloc(sizeof(uint64_t)*DramBandwidthBenchmarkDefaultMemCtrl_minimalPciStreamLength);

    struct timeval t1, t2;

    printf("warming up+setting up bistream...\n");
    fflush(stdout);
    DramBandwidthBenchmarkDefaultMemCtrl(384, outstream_sum);

    printf("\ntiming very short DFE run (what's the latency of running an action):");
    gettimeofday(&t1, NULL);
        DramBandwidthBenchmarkDefaultMemCtrl(384, outstream_sum);
    gettimeofday(&t2, NULL);

    double short_microSeconds = (t2.tv_sec - t1.tv_sec) * 1000 * 1000 + (t2.tv_usec - t1.tv_usec);
    double short_seconds = short_microSeconds/(1000.0*1000.0);
    printf("\ndone on DFE in %.2f seconds (%.2f microsec)\n", short_seconds, short_microSeconds);
    fflush(stdout);

    printf("\nbenchmarking: ");
    gettimeofday(&t1, NULL);
    for (int i = 0; i < repeatTimes; i++)
    {
        // vectorSize is size in bytes, internally this is split into 2 streams of 64bit (8byte) ints => 2*8 = 16
        DramBandwidthBenchmarkDefaultMemCtrl(ticksToRun, outstream_sum);
    }
    gettimeofday(&t2, NULL);

    double dfe_microSeconds = ((t2.tv_sec - t1.tv_sec) * 1000 * 1000 + (t2.tv_usec - t1.tv_usec))/repeatTimes;
    double dfe_seconds = dfe_microSeconds/(1000.0*1000.0);
    double dfe_bandwidth = datasetGB/dfe_seconds;
    printf("\ndone on DFE in %.2f seconds (%.2f microsec) per try in avg, in %d tries, bandwidth %.2f GB/s \n", dfe_seconds, dfe_microSeconds, repeatTimes, dfe_bandwidth);
    fflush(stdout);

    iffree(outstream_sum);

    return 0;
}
