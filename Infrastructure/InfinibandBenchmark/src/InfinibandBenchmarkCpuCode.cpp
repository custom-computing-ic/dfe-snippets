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


/** Maxmimum possible bandwidth at given clock speed*/
void printMaxInfinbandBandwidth() {
	double clock = 200.0;
	double inBitsPerCycle = 32 * 2;
	double outBitsPerCycle = 32;
	double max = (inBitsPerCycle + outBitsPerCycle) * clock * pow10(6) / GB;
	cout << "Max possible bandwidth " << max << " GB/s" << endl;
}


double measuredInifibandBandwidth(int size, long runtimeMs) {
	return 3 * size / GB / runtimeMs;
}


int main(void)
{

  const int inSize = 384 * 1E3;

  std::vector<int> a(inSize), b(inSize), expected(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected[i] = 2 * i;
  }

	printMaxInfinbandBandwidth();
  std::cout << "Running on DFE." << std::endl;
	struct timeval tv1, tv2;

	gettimeofday(&tv1, NULL);
  InfinibandBenchmark(inSize, &a[0], &b[0], &out[0]);
	gettimeofday(&tv2, NULL);
	long runtimeMs = ((tv2.tv_sec-tv1.tv_sec) * 1E6 +(tv2.tv_usec-tv1.tv_usec)) / 1000;		
	cout << "Runtime (ms) " << runtimeMs << endl;
	cout << "Bandwidth " << measuredInifibandBandwidth(inSize, runtimeMs) << endl;

  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected[i]);
      return 1;
    }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
