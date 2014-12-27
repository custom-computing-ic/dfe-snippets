/***
    Run the Kernel twice and check their outputs are different due
    to a presence of shared internal state.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 16;

  float *out = malloc(sizeof(float) * inSize);
  float *expected1 = malloc(sizeof(float) * inSize);
  float *expected2 = malloc(sizeof(float) * inSize);

  for(int i = 0; i < inSize; ++i)
  {
    expected1[i] = i + 1;
    expected2[i] = i + inSize;
  }

  printf("Running DFE first time.\n");
  int32_t firstRun = 1;
  BRAMPersistence(inSize, firstRun, out);

  int status = 1;
  for (int i = 0; i < inSize; i++)
  {
    printf("Output %d from DFE : %f, from CPU : %f", i, out[i], expected1[i]);

    if (fabs(out[i] - expected1[i]) > 1e-10)
    {
      printf(" -- did not match");
      status = 0;
    }
    printf("\n");
  }

  printf("Running DFE second time.\n");

  firstRun = 0;
  BRAMPersistence(inSize, firstRun, out);

  for (int i = 0; i < inSize; i++)
  {
    printf("Output %d from DFE : %f, from CPU : %f", i, out[i], expected2[i]);

    if (fabs(out[i] - expected2[i]) > 1e-10)
    {
      printf(" -- did not match");
      status = 0;
    }
    printf("\n");
  }


  if (status)
    printf("Test passed!\n");
  else
    printf("Test failed\n");
  return 0;
}
