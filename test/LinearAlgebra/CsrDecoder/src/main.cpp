#include <stdio.h>

#include <vector>
#include <iostream>

#include "CsrDecoder.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  std::vector<uint32_t> a{5, 6, 9, 10, 12, 13, 15, 16};
  std::vector<uint32_t> indptr(a[a.size() - 1]);
  std::vector<uint32_t> out(a.size(), 0);
  std::vector<uint32_t> exp;

  for (int i = 0; i < indptr.size(); i++) {
    indptr[i] = i;
  }

  exp.push_back(a[0]);
  for (int i = 1; i < a.size(); i++)
    exp.push_back(a[i] - a[i - 1]);


  std::cout << "Running on DFE." << std::endl;
  std::cout << "Size " << indptr.size() << std::endl;
  std::vector<uint32_t> indptrOut(indptr.size());
  CsrDecoder(
      a.size(), indptr.size(), 16,
      &a[0], &indptr[0], &indptrOut[0], &out[0]);

  int status = 0;
  for (int i = 0; i < a.size(); i++) {
    if (out[i] != exp[i]) {
      std::cout << "i: " << i << " got: " << out[i] << " exp: " << exp[i] << std::endl;
      status |= 1;
    }
  }

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
