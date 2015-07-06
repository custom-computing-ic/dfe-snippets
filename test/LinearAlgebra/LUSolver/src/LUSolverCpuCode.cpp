#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/VectorUtils.hpp>

using namespace std;
using namespace dfesnippets::blas;
using namespace dfesnippets::vectorutils;

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

  int s = 0;

  // transform A to upper diagonal matrix
  for (int i = 0; i < n; i++) {
    std::cout << " --- " << std::endl;
    for (int j = i + 1; j < n; j++) {
      double mij = a(j, i) / a(i, i);
      if (a(i, i) == 0) {
        cerr << " Error: Not all submatrices are singular" << endl;
      }
      a(j, i) = 0;
      b[j] -= b[i] * mij;
      for (int k = i + 1; k < n; k++) {
        a(j, k) -= mij * a(i, j);
      }
    }
    a.print();
  }

  // back substitution to solve for x
  return backSubstitue(a, b);
}

int main(void)
{
  int n = 4;

  double data[][n] = {
    {1, 1, 1, 1},
    {1, 2, 3, 3},
    {1, 1, 2, 2},
    {1, 1, 2, 3}};

  Matrix a{n};
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      a(i, j) = data[i][j];

  std::cout.precision(10);
  vector<double> b(n, 0);
  for (int i = 0; i < n; i++) {
    b[i] = i;
  }

  cout << "Initial: " << endl;
  a.print();

  vector<double> res = a * b;
  auto got = lusolve(a, res);

  cout << endl << "Final: " << endl;
  a.print();

  std::cout << "Rhs: " << std::endl;
  print_vector(res);
  std::cout << "Exp: " << std::endl;
  print_vector(b);
  std::cout << "Got: " << std::endl;
  print_vector(got);

  return 0;
}
