#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 32;

  std::vector<int> a(inSize), b(inSize), expected(inSize), out(inSize, 0);

  for(int i = 0; i < inSize; ++i)
  {
    a[i] = i + 1;
    b[i] = i;
  }

  const int storeDelay = 2;
  const int addedValue = 5;
  for(int i = 0; i < storeDelay; ++i)
  {
    expected[i] = addedValue;
  }
  for(int i = storeDelay; i < inSize; ++i)
  {
    expected[i] = a[i-storeDelay] + b[i-storeDelay] + addedValue;
  }

  std::cout << "Running on DFE." << std::endl;
  PackUnpack(inSize, &a[0], &b[0], &out[0]);


  /***
      Note that you should always test the output of your DFE
      design against a CPU version to ensure correctness.
  */
  for (int i = 0; i < inSize; i++)
  {
    printf("Output from DFE: i=%d, a=%d, b=%d, out=%d, expected=%d\n", i, a[i], b[i], out[i], expected[i]);
    if (out[i] != expected[i]) return 1;
  }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
