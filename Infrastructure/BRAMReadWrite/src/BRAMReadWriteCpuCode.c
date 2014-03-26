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

  const int inSize = BRAMReadWrite_dataSize;

  float *a = malloc(sizeof(float) * inSize);
  float *expected = malloc(sizeof(float) * inSize);
  float *out = malloc(sizeof(float) * inSize);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i;
    expected[i] = a[i] + 1;
  }

  printf("Running on DFE.\n");
  BRAMReadWrite(inSize, a, out);

  int status = 1;
  for (int i = 0; i < inSize; i++)
    if (fabs(out[i] - expected[i]) > 1e-10) {
      printf("Output from DFE did not match CPU: %d : %f != %f\n",
        i, out[i], expected[i]);
      status = 0;
    }

  if (status)
    printf("Test passed!\n");
  else
    printf("Test failed\n");
  return 0;
}
