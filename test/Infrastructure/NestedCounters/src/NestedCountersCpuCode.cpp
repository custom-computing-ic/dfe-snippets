/***
 * Two nested counters, in a triangular iteration space i.e.:
 * row: 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, etc.
 * col: 0, 0, 1, 0, 1, 2, 0, 1, 2, 3, etc.
*/

#include <stdio.h>

#include <vector>
#include <iostream>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void) {

  const int64_t ticks = 16;

  std::vector<int> out(ticks, 0);

  std::cout << "Running on DFE." << std::endl;
  NestedCounters(ticks, out.data(), ticks * sizeof(int));

  std::cout << "Output: ";
  for (int i = 0; i < ticks; i++)
    std::cout << out[i] << " ";
  std::cout << std::endl;
  return 0;
}
