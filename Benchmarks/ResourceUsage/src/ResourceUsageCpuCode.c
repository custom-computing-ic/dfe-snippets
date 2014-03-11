/***
    TODO: Add a descriptive comment!
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
  int *expected = malloc(sizeof(int) * inSize);

  int *intRes = malloc(sizeof(int) * inSize);
  float *spRes = malloc(sizeof(float) * inSize);
  double *dpRes = malloc(sizeof(double) * inSize);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected[i] = 2 * i;
  }

  printf("Running on DFE.\n");
  ResourceUsage(inSize, a, b, dpRes, intRes, spRes);

  for (int i = 0; i < inSize; i++) {
    int exp = 2 * a[i] + a[i] * b[i];
    if (intRes[i] != exp ||
	spRes[i] != exp ||
	dpRes[i] != exp) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n",
        i, intRes[i], exp);
      return 1;
    }
  }


  printf("Test passed!\n");
  return 0;
}
