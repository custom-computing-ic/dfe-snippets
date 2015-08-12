#include <stdio.h>

#include <vector>
#include <iostream>
#include <memory>

#ifdef PA_MAXELER
#include "Maxfiles.h"
#include "MaxSLiCInterface.h"
#endif

#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/VectorUtils.hpp>
#include <dfesnippets/sparse/utils.hpp>
#include <dfesnippets/FormatterUtils.hpp>

using namespace std;
using namespace dfesnippets::blas;
using namespace dfesnippets::vectorutils;
using namespace dfesnippets::formatting;
using namespace dfesnippets::utils;

// Solve the upper triangular system Ax = b using back substitution
vector<double> backSubstitute(const Matrix& a, vector<double> b) {
  auto n = b.size();
  vector<double> x(n, 0);
  for (int i = n - 1; i > 0; i--) {
    x[i] = b[i] / a(i, i);
    for (int j = i - 1; j > 0; j--) {
      b[j] -= x[i] * a(j, i);
    }
  }
  return x;
}

// solve Ax = b using Gaussian elimination with partial pivoting
// TODO
vector<double> lusolvepp(Matrix &a, vector<double> b) {
  auto n = a.size();

  // transform A to upper diagonal matrix
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (a(i, i) == 0) {
        std::cout << "Matrix before " << std::endl;
        a.print();
        double max = 0;
        int pivot = i;
        for (int k = i + 1; k < n; k++)
          if (abs(a(i, k)) > max) {
            max = abs(a(i, k));
            pivot = k;
          }
        a.row_interchange(i, pivot);
        double tmp = b[i];
        b[i] = b[pivot];
        b[pivot] = tmp;
        cout << " Info: Not all submatrices are singular - pivot ";
        cout << i << " <--> " << pivot << endl;
        std::cout << "Matrix after: " << std::endl;
        a.print();
        // TODO store permutations
      }
      double mij = a(j, i) / a(i, i);
      a(j, i) = 0;
      b[j] -= b[i] * mij;
      for (int k = i + 1; k < n; k++) {
        a(j, k) -= mij * a(i, k);
      }
    }
  }

  // back substitution to solve for x
  // TODO restore permutations
  return backSubstitute(a, b);
}


// solve Ax = b using Gaussian eliminiation without partial pivoting
// pre: for every k submatrix A[k:n][k:n] is nonsingular
vector<double> lusolve(const Matrix &a, vector<double> b) {
  int n = a.size();

  // transform A to upper diagonal matrix
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (dfesnippets::utils::almost_equal(a(i, i), 0)) {
        cerr << " Error: Not all submatrices are singular" << endl;
      }
      double mij = a(j, i) / a(i, i);
      a(j, i) = 0;
      b[j] -= b[i] * mij;
      for (int k = i + 1; k < n; k++) {
        a(j, k) -= mij * a(i, k);
      }
    }
  }

  // back substitution to solve for x
  return backSubstitute(a, b);
}


bool test_simple_ge(bool verbose = false) {
  long n = 100;

  ResultsFormatter rf(n * n * n  * 2.0 / 3.0, n * n);

  if (verbose)
    std::cout << "Building system: " << std::endl;
  System s(n);
  s.init();

  if (verbose) {
    cout << "Initial: " << endl;
    s.a.print_info();
  }

  if (verbose)
    std::cout << "Solving: " << std::endl;
  rf.startTiming();
  auto got = lusolve(s.a, s.b);
  rf.setCpuTime(rf.stopTiming());

  bool status = s.checkSolution(got);
  if (verbose)
    cout << endl << "Final: " << endl;
  //s.a.print();
  if (status) {
    std::cout << "Test passed " << std::endl;
  } else {
    std::cout << "Test failed " << std::endl;
  }
  if (verbose)
    rf.print();
  return status;
}

// test Gaussian elimination with partial pivoting
bool test_pp_ge(bool verbose = false) {
  long n = 10;

  ResultsFormatter rf(n * n * n  * 2.0 / 3.0, n * n);

  if (verbose)
    std::cout << "Building system: " << std::endl;
  GEPPSystem s(n);
  s.init();

  if (verbose) {
    cout << "Initial: " << endl;
    s.a.print_info();
    s.a.print();
  }

  if (verbose)
    std::cout << "Solving: " << std::endl;
  rf.startTiming();
  auto got = lusolvepp(s.a, s.b);
  rf.setCpuTime(rf.stopTiming());

  bool status = s.checkSolution(got);
  if (verbose)
    cout << endl << "Final: " << endl;
  if (status) {
    std::cout << "Test passed " << std::endl;
  } else {
    std::cout << "Test failed " << std::endl;
  }
  if (verbose)
    rf.print();
  return status;
}

int main(void) {
  bool passed = true;
//  passed &= test_simple_ge();
  passed &= test_pp_ge();
  return passed ? 0 : 1;
}
