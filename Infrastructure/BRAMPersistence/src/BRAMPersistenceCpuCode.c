/***
    Stream some floats and check they are stored, incremented and read
    properly from the kernel's local memory.
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

  printf("Running on DFE first time.\n");

  BRAMPersistence(inSize, out);

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

  printf("Running on DFE second time.\n");

  BRAMPersistence(inSize, out);

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
