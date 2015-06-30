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

  long n = 384 * 150;
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
  start = high_resolution_clock::now();
  DenseMatrixVectorMultiply_write(
      bsizeBytes * n,
      0,
      (uint8_t *)m.linear_access_pointer());

  // set mem contents to zero
  vector<double> zerov(n * partialSums, 0);
  DenseMatrixVectorMultiply_write(
      zerov.size() * sizeof(double),
      bsizeBytes * n,
      (uint8_t *)&zerov[0]);
  print_clock_diff("Write to DRAM took", start);

  cout << "Starting DFE run (" << iterations << " iterations) " << endl;
  start = high_resolution_clock::now();
  vector<double> copyv = dfesnippets::vectorutils::ncopy(v, iterations);

  DenseMatrixVectorMultiply(
      n,
      iterations,
      partialSums,
      &copyv[0]
      );
  print_clock_diff("FPGA run took", start);

  int size = n * partialSums;
  vector<double> b(size, 0);
  DenseMatrixVectorMultiply_read(
      bsizeBytes * partialSums,
      bsizeBytes * n,
      (uint8_t *)&b[0]);

  // Final round of reduction on CPU
  vector<double> res(n, 0);
  for (int i = 0; i < partialSums; i++)
    for (int j = 0; j < n; j++) {
      res[j] += b[i * n + j];
    }

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
