/***
    Here we test FifoWrapper. Note few values are lost due to FIFO overfill.
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
  const int inSize = 24;

  std::vector<int> a(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i)
  {
    a[i] = i + 1;
  }

  std::cout << "Running on DFE." << std::endl;
  FifoWrapper(inSize, &a[0], &out[0]);

  std::cout << std::endl;

  printf("\nDFE's output: ");
  for (int i = 0; i < inSize; i++)
  {
    printf(" %d", out[i]);
  }
  printf("\n");

  fflush(stdout);

  return 0;
}
