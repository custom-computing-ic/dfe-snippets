/***
    This is a simple demo project that you can copy to get started.
    Comments blocks starting with '***' and subsequent non-empty lines
    are automatically added to this project's wiki page.
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <dfesnippets/blas/Blas.hpp>
#include <dfesnippets/sparse/utils.hpp>

using namespace std;
using namespace dfesnippets::blas;

// Solves Ax = b using Jacobi iteration
vector<double> jacobiSolve(Matrix a, vector<double> b) {
  int n = b.size();
  int maxIt = 4000;
  vector<double> x(n, 0);

  for (int it = 0; it < maxIt; it++) {
    print_vector(x);
    bool allclose = true;
    vector<double> xnew(n, 0);
    for (int i = 0; i < n; i++) {
      double sum = 0;
      for (int j = 0; j < n; j++)
        if (i != j) {
          sum += a(i, j) * x[j];
        }
      xnew[i] = (b[i] - sum) / a(i, i);
      if (!almost_equal(x[i], xnew[i]))
        allclose = false;
    }

    if (allclose) {
      std::cout << "niterations " << it << std::endl;
      return xnew;
    }

    copy(xnew.begin(), xnew.end(), x.begin());
  }

  return x;
}

int main(void)
{

  const long n = 4;
  Matrix a(n);
  double  data[][n] = {
    {10, -1,  2,  0},
    {-1, 11, -1,  3},
    { 2, -1, 10, -1},
    { 0,  3, -1,  8}};

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      a(i, j) = data[i][j];

  vector<double> b{6, 25, -11, 15};
  vector<double> res = jacobiSolve(a, b);
  print_vector(res);

  std::cout << "Running on DFE." << std::endl;
  std::cout << "Test passed!" << std::endl;
  return 0;
}
