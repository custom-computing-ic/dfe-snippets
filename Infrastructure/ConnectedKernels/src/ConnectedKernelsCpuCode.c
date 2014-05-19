#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 384;

  int *a = malloc(sizeof(int) * inSize);
  int *b = malloc(sizeof(int) * inSize);
  int *expected = malloc(sizeof(int) * inSize);
  int *out = malloc(sizeof(int) * inSize);
  memset(out, 0, sizeof(int) * inSize);
  for(int i = 0; i < inSize; ++i)
  {
    a[i] = i;
    b[i] = i + 1;
  }

  for(int i = 0; i < inSize / 2; ++i)
  {
    expected[2*i + 0] = (a[i] + 1) * b[2*i + 0];
    expected[2*i + 1] = (a[i] + 1) * b[2*i + 1];
  }

  printf("Running on DFE.\n");
  ConnectedKernels(inSize, a, b, out);


  /***
      Note that you should always test the output of your DFE
      design against a CPU version to ensure correctness.
  */
  for (int i = 0; i < inSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
