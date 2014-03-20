/***
    Sends two 32 bit integer streams (3 times, apparently) and checks
    int, float and double results from FPGA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int intCpuResult(int a, int b)
{
    int sum  = a + b;
    int sub  = a - b;
    int prod = a * b;
    int div  = a / b;

    int imod1   = a % 3;          // = 2^2 - 1
    int imod2   = b % 32767;      // = 2^15 - 1
    int imod3   = b % 2147483647; // = 2^31 - 1 , 1345345345

    int iexp    = exp(b);
    int isqrt   = sqrt(b);
    int isin    = sin(b);
    int icos    = cos(b);
    int imax    = (a >= b)? a : b; // max(a, b);
    int imin    = (a >= b)? b : a; // min(a, b);
    int iabs    = abs(iexp);

    int intArithmetic  = sum + sub + prod + div;
    int intKernelMaths = 
                       iexp + isqrt + isin + icos + imax + imin + iabs +
                       imod1 + imod2 + imod3;
    return intArithmetic + intKernelMaths;
}

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
  ResourceUsage(inSize, a, a, a, b, b, b, dpRes, intRes, mpRes, spRes);

  float  spTol = 1e-8;
  double dpTol = 1e-16;

  for (int i = 0; i < inSize; i++) {
    int intExp = intCpuResult(a[i],b[i]);
    float exp = 2 * a[i] + a[i] * b[i];
    if (intRes[i] != intExp){
        printf("Integer output from DFE did not match CPU: %d : %d != %d\n", i, intRes[i], intExp);
        return 1;
    }
    if ((spRes[i] - exp) > spTol){
        printf("Single precision output from DFE did not match CPU: %d : %f != %f\n", i, spRes[i], exp);
        return 1;
    }
    if ((mpRes[i] - exp) > spTol){
        printf("Middle precision output from DFE did not match CPU: %d : %f != %f\n", i, mpRes[i], exp);
        return 1;
    }
    if ((dpRes[i] - exp) > dpTol){
        printf("Double precision output from DFE did not match CPU: %d : %f != %f\n", i, dpRes[i], exp);
        return 1;
    }
  }

  printf("Test passed!\n");
  return 0;
}
