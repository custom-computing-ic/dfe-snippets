#include <stdio.h>

#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include <omp.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"
#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/timing/Timing.hpp>
#include <dfesnippets/VectorUtils.hpp>

using namespace std;
using namespace std::chrono;
using namespace dfesnippets::blas;
using namespace dfesnippets::timing;
using namespace dfesnippets::vectorutils;

class ResultsFormatter {
  long flops, dataSize;

  double dramWriteTime = -1;
  double dramReadTime = -1;
  std::chrono::high_resolution_clock::time_point firstStart;
  std::chrono::high_resolution_clock::time_point start;

  bool timing = false;
  bool first = true;

  long dramReadSize, dramWriteSize;

  double computeTime;

  public:
    ResultsFormatter(
        long _flops,
        long _dataSize) : flops(_flops), dataSize(_dataSize) {
    }

    double toGBps(double bytes, double seconds) {
      return toGB(bytes) / seconds;
    }

    double toGB(double bytes) {
      return bytes / (1024.0 * 1024 * 1024);
    }

    double toGFlops(double flops) {
      return flops / 1E9;
    }

    void setDramWrite(double t, long sizeBytes) {
      this->dramWriteTime = t;
      this->dramWriteSize = sizeBytes;
    }

    void setDramRead(double t, long sizeBytes) {
      this->dramReadTime = t;
      this->dramReadSize = sizeBytes;
    }

    void setComputeTime(double t) {
      this->computeTime = t;
    }

    void print() {
      double gflops = toGFlops(flops);
      double totalRunTime = clock_diff(firstStart);

      if (dramReadTime != -1) {
        cout << "Dram Read" << endl;
        cout << "  Time (s)           = " << dramReadTime << endl;
        cout << "  Size (GB)          = " << toGB(dramReadSize) << endl;
        cout << "  Bwidth (GB/s)      = " << toGBps(dramReadSize, dramReadTime) << endl;
      }

      if (dramWriteTime != -1) {
        cout << "Dram Write " << endl;
        cout << "  Time (s)           = " << dramWriteTime << endl;
        cout << "  Size (GB)          = " << toGB(dramWriteSize) << endl;
        cout << "  Bwidth (GB/s)      = " << toGBps(dramWriteSize, dramReadTime) << endl;
      }

      cout << "Compute " << endl;
      cout <<   "  Time               = " << computeTime << endl;
      cout <<   "  GFLOPS             = " << gflops << endl;
      cout <<   "  GFLOPS/s           = " << gflops / computeTime << endl;
      cout <<   "  Bwidth (GB/s)      = " << toGBps(dataSize, computeTime) << endl;

      cout <<   "Running Total (s)    = " << totalRunTime << endl;
      cout <<   "Total GFLOPS         = " << gflops / totalRunTime << endl;
    }

    void startTiming() {
      if (timing) {
        cerr << "Timing already started, this is probably an error" << endl;
      }
      if (first) {
        first = false;
        firstStart = high_resolution_clock::now();
      }
      start = high_resolution_clock::now();
      timing = true;
    }

    // stops the timer and returns time elapsed (in ms) since the most recent startTiming call
    double stopTiming() {
      double diff = clock_diff(start);
      if (!timing) {
        cerr << "Timing not started, this is probably an error" << endl;
      }
      timing = false;
      return diff;
    }

};

vec run_cpu(const Matrix& m, const vec& b) {
  int n = m.size();
  // omp_set_num_threads(numThreads);
  auto start_time = high_resolution_clock::now();
  auto v = m * b;
  stringstream ss;
  ss << "Parallel (";
  ss << omp_get_max_threads();
  ss << " threads) ";
  print_clock_diff(ss.str(), start_time);
  return m * b;
}

int main(void) {

  long n = 48 * 8;//:w
  long iterations = 10;
  Matrix m(n);
  m.init_random();
  m.print_info();
 // m.print();

  vector<double> v(n, 1);
  auto exp = run_cpu(m, v);

  auto start = high_resolution_clock::now();
  m.convert_to_strided_access(48);
  print_clock_diff("Convert to strided", start);

  max_config_set_int64(MAX_CONFIG_PCIE_TIMEOUT, 120);

  int partialSums = 8 * 4 * 2;

  long bsizeBytes = sizeof(double) * n;

  long stripeWidth = 48;
  ResultsFormatter rf(2 * n * n, 2 * n * n / stripeWidth);
  rf.startTiming();
  DenseMatrixVectorMultiply_write(
      bsizeBytes * n,
      0,
      (uint8_t *)m.linear_access_pointer());
  rf.setDramWrite(rf.stopTiming(), bsizeBytes * n);

  // set mem contents to zero
  vector<double> zerov(n * partialSums, 0);
  DenseMatrixVectorMultiply_write(
      zerov.size() * sizeof(double),
      bsizeBytes * n,
      (uint8_t *)&zerov[0]);
  print_clock_diff("Write to DRAM", start);

  cout << "Starting DFE run (" << iterations << " iterations) " << endl;
  start = high_resolution_clock::now();
  vector<double> copyv = dfesnippets::vectorutils::ncopy(v, iterations);

  rf.startTiming();
  DenseMatrixVectorMultiply(
      n,
      iterations,
      partialSums,
      &copyv[0]
      );
  rf.setComputeTime(rf.stopTiming());

  int size = n * partialSums;
  vector<double> b(size, 0);

  rf.startTiming();
  DenseMatrixVectorMultiply_read(
      bsizeBytes * partialSums,
      bsizeBytes * n,
      (uint8_t *)&b[0]);
  rf.setDramRead(rf.stopTiming(), bsizeBytes * partialSums);

  print_vector(exp);
  // Final round of reduction on CPU
  vector<double> res(n, 0);
  for (int i = 0; i < partialSums; i++)
    for (int j = 0; j < n; j++) {
      res[j] += b[i * n + j];
    }
  print_vector(res);

  rf.print();

  for (int j = 0; j < iterations; j++) {
    for (int i = 0; i < n; ++i) {
      if (res[j] != exp[i] * iterations) {
        cout << "Wrong output, iteration " << j << endl;
        return 1;
      }
    }
  }

  cout << "Test passed" << endl;
  return 0;
}
