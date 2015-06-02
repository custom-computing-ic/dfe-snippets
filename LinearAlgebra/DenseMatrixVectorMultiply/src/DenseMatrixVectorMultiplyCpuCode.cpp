#include <stdio.h>

#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include <omp.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

using namespace std;
using namespace std::chrono;

typedef double value_t;
typedef vector<value_t> vec;

class Matrix {
  long n;
  value_t* data;

public:
  Matrix(long _n) : n(_n) {
    data = new value_t[n * n];
  }

  ~Matrix() {
    delete [] data;
  }

  value_t& operator()(long i, long j) {
    return data[i * n + j];
  }

  vec operator* (const vec& v) const {
    vec r(n, 0);
    for (long i = 0; i < n; i++)
      for (long j = 0; j < n; j++) {
        r[i] += v[j] * data[i * n + j];
      }
    return r;
  }

  void print() {
    for (int i = 0; i < n; i ++ ) {
      for (int j = 0 ; j < n; j++)
        cout << operator()(i, j) << " ";
      std::cout << std::endl;
    }
  }

  void init_random() {
    for (long i = 0; i < n; i ++ )
      for (long j = 0 ; j < n; j++)
//        data[i * n + j] = ((j / 48) + 1) * 10 + i % 10;
        data[i * n + j] =  j / 48 + 1;
  }

  void print_info() {
    cout << "Matrix "  << endl;
    cout << "  Dense" << endl;
    cout << "  Size (GB) " << n * n  * 8 / (1024 * 1024 * 1024) << endl;
  }

  value_t* linear_access_pointer() {
    return data;
  }

  inline int size() const {
    return n;
  }
};

void print_clock_diff(
    string item,
    high_resolution_clock::time_point start) {
  auto end = high_resolution_clock::now();
  cout << item << " took ";
  cout << duration_cast<duration<double> >(end - start).count();
  cout << " seconds." << endl;
}

vec run_parallel(const Matrix& m, const vec& b, const int numThreads) {
  int n = m.size();
  omp_set_num_threads(numThreads);
  auto start_time = high_resolution_clock::now();
#pragma omp parallel for
  for (int i = 0; i < 10; i++) {
    auto v = m * b;
  }
  stringstream ss;
  ss << "Parallel (";
  ss << omp_get_max_threads();
  ss << " threads) ";
  print_clock_diff(ss.str(), start_time);
  return m * b;
}

int main(void) {

  long n = 96 * 96;
  Matrix m(n);
  m.init_random();
  m.print_info();
//  m.print();

  vector<double> b(n, 0);
  vector<double> v(n, 1);
  auto exp = run_parallel(m, v, 1);
  //run_parallel(m, 2);
  //run_parallel(m, 6);
  //run_parallel(m, 12);
  //run_parallel(m, 24);

  //std::cout << "Running on DFE." << std::endl;
  long bsizeBytes = sizeof(double) * n;

  DenseMatrixVectorMultiply_write(
      bsizeBytes * n,
      bsizeBytes,
      (uint8_t *)m.linear_access_pointer());

  DenseMatrixVectorMultiply(n, &v[0], &b[0]);

  for (int i = 0; i < n; ++i) {
    if (b[i] != exp[i]) {
      cout << "Wrong output " << endl;
      return 1;
    }
  }

  cout << "Test passed" << endl;
  return 0;
}
