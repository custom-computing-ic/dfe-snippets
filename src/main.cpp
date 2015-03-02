#include <stdio.h>
#include <omp.h>

#include <vector>
#include <iostream>
#include <cmath>
#include <tuple>
#include <fstream>
#include <chrono>
#include <algorithm>

#include <utils.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include "Cpu.h"
#include "Fpga.h"

using namespace std;
using namespace std::chrono;

bool check(vector<float> expected,
           vector<float> got) {
  auto result = mismatch(expected.begin(), expected.end(), got.begin());
  if (result.first != expected.end()) {
    cout << "   At " << result.first - expected.begin() << " ";
    cout << "   Expected: " << *(result.first) << " got: " << *(result.second) << endl;
    return false;
  }
  return true;
}

int runBenchmark(int vectorSize) {

  cout << "   Parameters.OMP_THREADS      = " << omp_get_max_threads() << endl;
  cout << "   Parameters.PIPES        = " << SpmvBase_numPipes << endl;
  cout << "   Parameters.STREAM_FREQ = " << SpmvBase_streamFreq << endl;

  if (vectorSize % 384 != 0) {
    cerr << "vector size must be a multiple of 384" << endl;
    exit(1);
  }

  vector<float> a(vectorSize), b(vectorSize);
  vector<float> sum(0, vectorSize), diff(0, vectorSize);
  
  for (size_t i = 0; i < a.size(); i++) {
    a[i] = rand() % 100;
    b[i] = rand() % 100;
  }

  cout << "Running CPU benchmark" << endl;
  auto start_time = high_resolution_clock::now();
  auto expected = runCPU(a, b);
  double tookS = clock_diff(start_time);
  double fpgaClockFrequency = 200 * 1024.0 * 1024.0;
  double estimated = vectorSize / (fpgaClockFrequency * SpmvBase_numPipes);
  start_time = high_resolution_clock::now();
  writeDRAM(a);
  double dramS = clock_diff(start_time);

  cout << "Running DFE" << endl;
  start_time = high_resolution_clock::now();
  auto fpgaResult = runFPGA(a, b);
  double fpgaS = clock_diff(start_time);
  double baseSpeedup = tookS / fpgaS;

  cout << "   Results.CPU_TIME       = " << tookS << " s" << endl;
  cout << "   Results.DRAM_WRITE     = " << dramS << " s" << endl;
  cout << "   Results.DFE_ESTIMATED = " << estimated << " s" << endl;
  cout << "   Results.DFE_ACTUAL    = " << fpgaS << " s" << endl;
  cout << "   Results.SPEEDUP        = " << baseSpeedup << endl;

  cout << "Checking results" << endl;
  bool r = check(expected.first, fpgaResult.first);
  r &= check(expected.second, fpgaResult.second);
  
  if (r) {
    cout << "  All correct! " << endl;
    return 0;
  }

  cout << "  Results mismatch " << endl;
  return 1;
}

int main(int argc, char** argv) {
  cout << "Program arguments:" << endl;
  for (int i = 0; i < argc; i++)
    cout << "   " << argv[i] << endl;

  if (argc == 5 && string(argv[3]) == "-s" ) {
      cout << "Running benchmark" << endl;
      int vectorSize = atoi(argv[4]);
      return runBenchmark(vectorSize);
  } else {
    cout << "Wrong args" << endl;
    cout << "Usage: bash ../run-sim.sh bin/fpga_sim <path to expr> <path to market data>" << endl;
    return 1;
  }
  return 0;
}
