#include <stdio.h>

#include <vector>
#include <iostream>

#include "DramAccumulator.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 384 * 70;

  std::vector<int> a(inSize), expected(inSize), out(inSize, 0);

  int iterations = 2;
  int k = 5;
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    expected[i] = a[i] + k * iterations;
  }

  std::cout << "Running on DFE." << std::endl;
  DramAccumulator_write(
      a.size() * sizeof(int),
      0,
      (uint8_t *)&a[0]);
  DramAccumulator(a.size(), iterations, k);
  DramAccumulator_read(
      out.size() * sizeof(int),
      0,
      (uint8_t *)&out[0]);

  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : Got %d != Exp %d\n",
        i, out[i], expected[i]);
      return 1;
    }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
