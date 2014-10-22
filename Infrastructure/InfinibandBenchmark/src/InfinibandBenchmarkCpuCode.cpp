/***
 * Infiniband maximum achievable bandwidth test.
 */

#include <stdio.h>

#include <vector>
#include <iostream>
#include <cmath>

#include "InfinibandBenchmark.h"
#include "MaxSLiCInterface.h"

using namespace std;

// All measurements should be in bytes
const double GB = pow(1024, 3);
const double MB = pow(1024, 2);


/** Maxmimum possible bandwidth at given clock speed*/
void printMaxInfinbandBandwidth() {
    double clock = 150.0;
    double inBytesPerCycle = sizeof(long) * 2;
    double outBytesPerCycle = sizeof(long) * 2;
    double max = (inBytesPerCycle + outBytesPerCycle) * clock * 1E6 / GB;
    cout << "Max possible bandwidth (@150 MHz Stream Clock): " << max << " GB/s" << endl;
}


double measuredInifibandBandwidth(long sizeInBytes, double runtimeS) {
    return sizeInBytes / MB / runtimeS;
}


int check(long got, long expected, int i, int streamNo) {
    if (got != expected) {
        printf("Output from DFE did not match CPU (stream: %d): %d : %d != %d\n",
               streamNo, i, got, expected);
        return 1;
    }
    return 0;
}


int main(void) {

    const long inSize = 2 * 384 * 1E3;
    const long dataSizeBytes = 4 * inSize * sizeof(long);

    vector<long> a(inSize), b(inSize), c(inSize), d(inSize), expected(inSize);
    vector<long> out1(inSize, 0), out2(inSize, 0), out3(inSize, 0), out4(inSize, 0);

    for(long i = 0; i < inSize; ++i) {
        a[i] = i + 1;
        b[i] = i - 1;
        c[i] = i + 1;
        d[i] = i - 1;
        expected[i] = 2 * i;
    }

    printMaxInfinbandBandwidth();
    cout << "Running on DFE. Data size: " << dataSizeBytes / GB << " GB" << endl;

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    InfinibandBenchmark(inSize,
                        &a[0], &b[0], &c[0], &d[0],
                        &out1[0], &out2[0], &out3[0], &out4[0]);
    gettimeofday(&tv2, NULL);

    double runtimeS = ((tv2.tv_sec-tv1.tv_sec) * (double)1E6 +
                       (tv2.tv_usec-tv1.tv_usec)) / (double)1E6;

    cout << "Runtime (s) " << runtimeS << endl;
    cout << "Bandwidth " << measuredInifibandBandwidth(dataSizeBytes,  runtimeS);
    cout << " MB/s " << endl;

    for (int i = 0; i < inSize; i++) {
        check(out1[i], expected[i], i, 1);
        check(out2[i], 2, i, 2);
        check(out3[i], expected[i], i, 3);
        check(out4[i], 2, i, 4);
    }

    std::cout << "Test passed!" << std::endl;
    return 0;
}
