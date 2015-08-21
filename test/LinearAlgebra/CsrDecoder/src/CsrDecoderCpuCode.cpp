#include <stdio.h>

#include <vector>
#include <iostream>

#include "CsrDecoder.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  // set up two matrices,
  // of 8 rows/columns each and different number of nonzeros
  int nrows = 8;
  int n1 = 16;
  int n2 = 32;
  std::vector<uint32_t> a{
    5, 6, 9, 10, 12, 13, 15, 16, // first matrix, 16 nonzeros
    2, 3, 10, 12, 14, 16, 18, 32 // second matrix, 32 nonzeros
  };

  std::vector<uint32_t> indptr(n1 + n2);
  std::vector<uint32_t> exp;

  for (int i = 0; i < indptr.size(); i++) {
    indptr[i] = i;
  }

  exp.push_back(a[0]);
  int i = 1;
  for (; i < nrows; i++)
    exp.push_back(a[i] - a[i - 1]);
  exp.push_back(a[i++]);
  for (; i < a.size(); i++)
    exp.push_back(a[i] - a[i - 1]);

  std::cout << "Running on DFE." << std::endl;
  std::cout << "Size " << indptr.size() << std::endl;
  std::vector<uint32_t> indptrOut(indptr.size(), 0);
  std::vector<uint32_t> out(a.size(), 0);
  CsrDecoder(
      a.size(), indptr.size(), nrows,
      &a[0], &indptr[0], &indptrOut[0], &out[0]);

  int status = 0;
  std::cout << "Checking row lengths" << std::endl;
  for (int i = 0; i < a.size(); i++) {
    if (out[i] != exp[i]) {
      std::cout << "i: " << i << " got: " << out[i] << " exp: " << exp[i] << std::endl;
      status |= 1;
    }
  }

  std::cout << "Checking indptr"  << std::endl;
  for (int i = 0; i < indptr.size(); i++) {
    if (indptr[i] != indptrOut[i]) {
      std::cout << "i: " << i << " got: " << indptr[i] << " exp: " << indptrOut[i] << std::endl;
      status |= 1;
    }
  }

  if (status == 0)
    std::cout << "Test passed!" << std::endl;
  return status;
}
