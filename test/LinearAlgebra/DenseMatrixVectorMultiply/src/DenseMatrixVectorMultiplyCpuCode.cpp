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

  long n = 200 * 384; // 3 * (1 << 14);
  Matrix m(n);
  m.init_random();
  m.print_info();
 // m.print();

  vector<double> b(n, 0);
  vector<double> v(n, 1);
  auto exp = run_cpu(m, v);

  auto start = high_resolution_clock::now();
  m.convert_to_strided_access(48);
  print_clock_diff("Convert to strided", start);

  long bsizeBytes = sizeof(double) * n;
  start = high_resolution_clock::now();
  DenseMatrixVectorMultiply_write(
      bsizeBytes * n,
      0,
      (uint8_t *)m.linear_access_pointer());
  print_clock_diff("Write to DRAM took", start);

  cout << "Starting DFE run " << endl;
  start = high_resolution_clock::now();
  DenseMatrixVectorMultiply(
      n,
      &v[0],
      &b[0]);
  print_clock_diff("FPGA run took", start);

  for (int i = 0; i < n; ++i) {
    if (b[i] != exp[i]) {
      cout << "Wrong output " << endl;
      return 1;
    }
  }

  cout << "Test passed" << endl;
  return 0;
}
