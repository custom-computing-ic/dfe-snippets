#include <stdio.h>

#include <vector>
#include <iostream>
#include <memory>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/VectorUtils.hpp>
#include <dfesnippets/sparse/utils.hpp>
#include <dfesnippets/FormatterUtils.hpp>

using namespace std;
using namespace dfesnippets::blas;
using namespace dfesnippets::vectorutils;
using namespace dfesnippets::formatting;

class System {
  public:
    int n;
    Matrix a;
    vector<double> x;
    vector<double> b;

    System(int _n) : n(_n), a(_n), x(_n, 0), b(_n, 0) {
      a.init(0);
    }

    void init() {
      // start with identity matrix
      // fill the first row with multiples of main diagonal elements
      for (int i = 0; i < n; i++) {
        a(0, i) = i % 4;
        a(i, i) = 1;
      }

      fillMatrix();

      // generate solution
      for (int i = 0; i < n; i++)
        x[i] = i % 20;

      // generate rhs
      b = a * x;
    }

    bool checkSolution(vector<double> got) {
      bool good = true;
      for (size_t i = 0; i < x.size(); i++)
        if (!almost_equal(got[i], x[i], 1E-10)) {
          cerr << "Got " << got[i] << " exp " << x[i] << " @i= " << i << endl;
          good = false;
        }
      return good;
    }

    virtual void fillMatrix() {
      // apply some of the basic operations, adding a multiple of the first row to
      // all other rows
      for (int i = 1; i < n; i++) {
        for (int k = 0; k < n; k++) {
          a(i, k) += i * ((k % 4) + 1) * a(0, k);
        }
      }
    }
};

// A system which must be solved with (at least) partial pivoting
class GEPPSystem : public System {

  public:
  GEPPSystem(long _n) : System(_n) {}

  virtual void fillMatrix() override {
    // for the system to require pivoting, at least one row must have zero
    // entries on the main diagonal: we don't fill the last row and just interchange
    // it with another row at the end
    for (int i = 1; i < n - 1; i++) {
      for (int k = 0; k < n; k++) {
        a(i, k) += i * ((k % 4) + 1) * a(0, k);
      }
    }

    if (n - 1 == n / 2) {
      cerr << "Error: can't build a nice GEPP System with n of size " << n << endl;
    }
    a.row_interchange(n - 1, n / 2);
  }
};

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
vector<double> lusolvepp(const Matrix &a, vector<double> b) {
  auto n = a.size();

  // transform A to upper diagonal matrix
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      double mij = a(j, i) / a(i, i);
      if (a(i, i) == 0) {
        cout << " Warning: Not all submatrices are singular - finding pivot" << endl;
        // TODO find row with largest element
        // TODO interchange
        // TODO store permutations
      }
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
      if (almost_equal(a(i, i), 0)) {
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


void test_simple_ge(bool verbose = false) {
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

  if (verbose)
    cout << endl << "Final: " << endl;
  //s.a.print();
  if (s.checkSolution(got)) {
    std::cout << "Test passed " << std::endl;
  } else {
    std::cout << "Test failed " << std::endl;
  }
  if (verbose)
    rf.print();
}

// test Gaussian elimination with partial pivoting
void test_pp_ge(bool verbose = false) {
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
  auto got = lusolve(s.a, s.b);
  rf.setCpuTime(rf.stopTiming());

  if (verbose)
    cout << endl << "Final: " << endl;
  //s.a.print();
  if (s.checkSolution(got)) {
    std::cout << "Test passed " << std::endl;
  } else {
    std::cout << "Test failed " << std::endl;
  }
  if (verbose)
    rf.print();
}

int main(void) {
  test_simple_ge();
  test_pp_ge();
  return 0;
}
