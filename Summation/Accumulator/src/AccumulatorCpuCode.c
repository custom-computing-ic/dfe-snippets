/***
    CPU code for a minimal benchmark of the Maxeler Accumulator API.
*/

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
  int *expected_int = malloc(sizeof(int) * inSize);
  int *out = malloc(sizeof(int) * inSize);

  memset(out, 0, sizeof(int) * inSize);
  memset(expected_int, 0, sizeof(int) * inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected_int[i] = a[i] + (i > 0 ? expected_int[i - 1] : 0);
  }

  printf("Running on DFE.\n");
  Accumulator(inSize, a, out);


  for (int i = 0; i < inSize; i++)
    if (out[i] != expected_int[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, out[i], expected_int[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
