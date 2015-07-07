#include <stdio.h>

#include <vector>
#include <iostream>
#include <memory>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/VectorUtils.hpp>
#include <dfesnippets/sparse/utils.hpp>

using namespace std;
using namespace dfesnippets::blas;
using namespace dfesnippets::vectorutils;

class System {
  public:
    Matrix a;
    vector<double> x;
    vector<double> b;
    int n;
    System(int _n) : n(_n), a(_n), x(_n, 0), b(_n, 0) {
      a.init(0);
      // start with identity matrix
      // fill the first row with multiples of main diagonal elements
      for (int i = 0; i < n; i++) {
        a(0, i) = i % 4;
        a(i, i) = 1;
      }

      // apply some of the basic operations, adding a multiple of the first row to
      // all other rows
      for (int i = 1; i < n; i++) {
        for (int k = 0; k < n; k++) {
          a(i, k) += i * ((k % 4) + 1) * a(0, k);
        }
      }

      // generate solution
      for (int i = 0; i < n; i++)
        x[i] = i % 20;

      // generate rhs
      b = a * x;

    }

    bool checkSolution(vector<double> got) {
      bool good = true;
      for (int i = 0; i < x.size(); i++)
        if (!almost_equal(got[i], x[i], 1E-8)) {
          cerr << "Got " << got[i] << " exp " << x[i] << " @i= " << i << endl;
          good = false;
        }
      return good;
    }
};


// solve Ax = b using Gaussian elimination with partial pivoting
// TODO
vector<double> lusolvepp(Matrix a, vector<double> b) {

  // lu decomposition

  // backward substitution

  return b;
}

vector<double> backSubstitue(const Matrix& a, vector<double> b) {
  int n = b.size();
  vector<double> x(n, 0);
  for (int i = n - 1; i > 0; i--) {
    x[i] = b[i] / a(i, i);
    for (int j = i - 1; j > 0; j--) {
      b[j] -= x[i] * a(j, i);
    }
  }
  return x;
}

// solve Ax = b using Gaussian eliminiation without partial pivoting
// pre: for every k submatrix A[k:n][k:n] is nonsingular
vector<double> lusolve(const Matrix &a, vector<double> b) {
  int n = a.size();

  // transform A to upper diagonal matrix
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      double mij = a(j, i) / a(i, i);
      if (a(i, i) == 0) {
        cerr << " Error: Not all submatrices are singular" << endl;
      }
      a(j, i) = 0;
      b[j] -= b[i] * mij;
      for (int k = i + 1; k < n; k++) {
        a(j, k) -= mij * a(i, k);
      }
    }
  }

  // back substitution to solve for x
  return backSubstitue(a, b);
}

int main(void) {

  int n = 4000;

  std::cout << "Building system: " << std::endl;
  System s(n);

  cout << "Initial: " << endl;
  s.a.print_info();

  std::cout << "Solving: " << std::endl;
  auto got = lusolve(s.a, s.b);

  cout << endl << "Final: " << endl;
  //s.a.print();

  if (s.checkSolution(got)) {
    std::cout << "Test passed " << std::endl;
    return 0;
  }

  std::cout << "Test failed " << std::endl;
  //std::cout << "Rhs: " << std::endl;
  //print_vector(s.b);
  //std::cout << "Exp: " << std::endl;
  //print_vector(s.x);
  //std::cout << "Got: " << std::endl;
  //print_vector(got);

  return 1;
}
