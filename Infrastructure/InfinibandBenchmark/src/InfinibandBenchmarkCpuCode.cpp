/***
 * Infiniband maximum achievable bandwidth test.
 */

#include <stdio.h>

#include <vector>
#include <iostream>
#include <cmath>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

using namespace std;

const double GB = pow(1024, 3) * 8;
const double MB = pow(1024, 2) * 8;


/** Maxmimum possible bandwidth at given clock speed*/
void printMaxInfinbandBandwidth() {
    double clock = 200.0;
    double inBitsPerCycle = 32 * 2;
    double outBitsPerCycle = 32;
    double max = (inBitsPerCycle + outBitsPerCycle) * clock * pow10(6) / GB;
    cout << "Max possible bandwidth " << max << " GB/s" << endl;
}


double measuredInifibandBandwidth(long sizeInBits, double runtimeS) {
    return 3 * size / MB / runtimeS;
}


int main(void)
{

    const long inSize = 2 * 384 * 1E6;

    std::vector<int> a(inSize), b(inSize), expected(inSize), out(inSize, 0);

    for(long i = 0; i < inSize; ++i) {
        a[i] = i + 1;
        b[i] = i - 1;
        expected[i] = 2 * i;
    }

    printMaxInfinbandBandwidth();
    cout << "Running on DFE." << endl;
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    InfinibandBenchmark(inSize, &a[0], &b[0], &out[0]);
    gettimeofday(&tv2, NULL);

    cout << "Data size " << inSize * sizeof(int) / GB  << " GBs" << endl;
    double runtimeS = ((tv2.tv_sec-tv1.tv_sec) * (double)1E6 +
                       (tv2.tv_usec-tv1.tv_usec)) / (double)1E6;
    cout << "Runtime (s) " << runtimeS << endl;
    cout << "Bandwidth " << measuredInifibandBandwidth(inSize * sizeof(int),  runtimeS) << " MB/s " << endl;

    for (int i = 0; i < inSize; i++)
        if (out[i] != expected[i]) {
            printf("Output from DFE did not match CPU: %d : %d != %d\n",
                   i, out[i], expected[i]);
            return 1;
        }

    std::cout << "Test passed!" << std::endl;
    return 0;
}
