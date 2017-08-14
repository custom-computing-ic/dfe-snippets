#include <stdio.h>

#include <vector>
#include <iostream>
#include <iomanip>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

using namespace std;

// lower triangular system matrix
struct system_t {
  int n;
  vector<float> nums;
};

template<typename T>
int64_t sizeBytes(const vector<T>& t) {
  return t.size() * sizeof(T);
}

template<typename T>
void print(const vector<T>& t) {
  for (auto v : t) {
    cout << v << " ";
  }
  cout << endl;
}

int64_t countCycles(const system_t& s) {
  const int latency = 11 + 48 + 28 + 12 + 1 + 12 + 5;
  return s.nums.size() + s.n * latency;
}


// Solve Ax = b, with A a triangular system
void triangularSolve(const system_t& a, vector<float>& x, const vector<float>& b) {

  int n = a.n;
  int pos = 0;

  for (int i = 0; i < n; i ++) {
    float result = 0;
    for (int j = 0; j < i; j++) {
      result += x[j] * a.nums[pos++];
    }
    x[i] = (b[i] - result) / a.nums[pos++];
  }

}

int main(void) {

  // NB must be a multiple of 8
  const int inSize = 16;

  std::vector<float> a, x(inSize, 0), b, out(inSize, 0);

  float val = 1;
  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < i + 1; j++)  {
      a.push_back(val);
      // std::cout << val << " ";
      val += 2;
      if (val > 10)
        val = 1;
    }
    std::cout << endl;
    b.push_back(i + 1);
  }

  system_t sys;
  sys.n = inSize;
  sys.nums = a;

  triangularSolve(sys, x, b);

  std::cout << "Running on DFE." << std::endl;
  int nCycles = countCycles(sys);
  std::cout << " Cycles = " << nCycles << std::endl;
  TriangularSolver(nCycles, &a[0], sizeBytes(a), &b[0], sizeBytes(b), &out[0], sizeBytes(out));

  cout << "          Exp             Got" << endl;
  cout << "          ---             ---" << endl;
  for (int i = 0; i < inSize; i++) {
    cout << fixed << setprecision(5) << setw(15) << x[i] << "   " << setw(15) << out[i] << endl;
  }

  return 0;
}
