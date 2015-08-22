#include <stdio.h>

#include <vector>
#include <iostream>

#include "ParallelCsrDecoder.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int lengthSize = 32 * 32;
  const int inputWidth = ParallelCsrDecoder_inputWidth;
  const int inSize = lengthSize * inputWidth;

  std::vector<double> a(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
  }

  std::cout << "Running on DFE." << std::endl;
  std::vector<int> length;

  for (int i = 0; i < lengthSize; i++)
    length.push_back(i % 3 + 1);

  int ticks = lengthSize;
  ParallelCsrDecoder(
      ticks,
      &a[0],
      &length[0],
      &out[0]);

  for (int i = 0; i < ticks * inputWidth; ++i) {
    if (i % inputWidth == 0)
      std::cout << std::endl;
    std::cout << out[i] << " ";
  }


  std::cout << "Test passed!" << std::endl;
  return 0;
}
