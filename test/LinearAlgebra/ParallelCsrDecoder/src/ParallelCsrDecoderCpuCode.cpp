#include <stdio.h>

#include <vector>
#include <iostream>

#include "ParallelCsrDecoder.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 384;

  std::vector<int> a(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
  }

  std::cout << "Running on DFE." << std::endl;
  ParallelCsrDecoder(inSize, &a[0], &out[0]);

  for (int i = 0; i < inSize; i++)
    if (out[i] != a[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], a[i]);
      return 1;
    }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
