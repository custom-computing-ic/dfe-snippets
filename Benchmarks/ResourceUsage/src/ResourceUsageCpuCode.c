/***
    Sends two 32 bit integer streams (3 times, apparently) and checks
    int, float and double results from FPGA.
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

  int *intRes = malloc(sizeof(int) * inSize);
  float *spRes = malloc(sizeof(float) * inSize);
  float *mpRes = malloc(sizeof(float) * inSize);
  double *dpRes = malloc(sizeof(double) * inSize);

  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
  }

  printf("Running on DFE.\n");
  ResourceUsage(inSize, a,a,a, b,b,b, dpRes, intRes, mpRes, spRes);

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
