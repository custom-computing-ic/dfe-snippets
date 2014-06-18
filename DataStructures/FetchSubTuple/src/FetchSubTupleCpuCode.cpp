/***
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
  const int cycles = 40;
  const int inSize = cycles*FetchSubTuple_tupleSize;

  std::vector<int> a(inSize), out(inSize, 0);

  int sizes[] = {1,2,1,2, 3,1,2,1, 3,1,2,7, 7,7,8,7, 8,5,1,3, 6,5,8,7, 7,7,8,7, 8,7,7,7, 8,7,8,7, 7,7,8,7, 8,7,7,7, 8,7,8,7};

  int numElements = 0;
  for(int i = 0; i < inSize; ++i)
  {
    a[i] = i + 1;

    // the kernel skips 3 cycles => 40-3 = 37
    if (i < 37) numElements += sizes[i];
  }

  std::cout << "Running on DFE." << std::endl;
  FetchSubTuple(cycles, inSize, &a[0], sizes, &out[0]);

  std::cout << std::endl;

  int count = 0;
  for (int i = 0; i < inSize/FetchSubTuple_tupleSize; i++)
  {
    printf("\ntuple %d with size %d : ", i, sizes[i]);
    for (int j = 0; j < FetchSubTuple_tupleSize; j++)
    {
        printf("%d ", out[count+j]);
    }
    count += FetchSubTuple_tupleSize;
  }

  std::cout << std::endl << std::endl << "number of elements to be fetched in total: " << numElements << std::endl;

  fflush(stdout);

  std::cout << std::endl << "Test passed!" << std::endl;
  return 0;
}
