/***
    This is a simple demo project that you can copy to get started.
    Comments blocks starting with '***' and subsequent non-empty lines
    are automatically added to this project's wiki page.
*/

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const uint32_t inSize = 384;

  std::vector<uint32_t> in(inSize), out0(inSize, 0), out1(inSize, 0);

  for(uint32_t i = 0; i < inSize; ++i) {
    in[i] = i;
  }

  std::cout << "Writing data to LMem." << std::endl;
  LMemMultiChannel_Write(inSize, &in[0]);

  std::cout << "Running DFE." << std::endl;
  LMemMultiChannel_Exec(inSize, &out0[0], &out1[0]);

  /***
      Note that you should always test the output of your DFE
      design against a CPU version to ensure correctness.
  */
  for (uint32_t i = 0; i < inSize; i++)
    if (in[i] != out0[i] && in[i] != out1[i]) {
      printf("Output from DFE did not match CPU\n");
      return 1;
    }

  std::cout << "Test passed!" << std::endl;
  return 0;
}
