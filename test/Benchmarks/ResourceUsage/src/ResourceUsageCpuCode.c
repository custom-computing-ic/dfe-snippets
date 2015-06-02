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

int32_t intCpuResult(int a, int b)
{
    int32_t sum  = a + b;
    int32_t sub  = a - b;
    int32_t prod = a * b;
    int32_t div  = a / b;

    int32_t imod1   = a % 3;          // = 2^2 - 1
    int32_t imod2   = b % 32767;      // = 2^15 - 1
    int32_t imod3   = b % 2147483646; // = 2^31 - 1

    int32_t iexp    = exp(b);
    int32_t isqrt   = sqrt(b);
    int32_t isin    = sin(b);
    int32_t icos    = cos(b);
    int32_t imax    = (a >= b)? a : b; // max(a, b);
    int32_t imin    = (a >= b)? b : a; // min(a, b);
    int32_t iabs    = abs(iexp);

    int32_t intArithmetic  = sum + sub + prod + div;
    int32_t intKernelMaths = 
                       iexp + isqrt + isin + icos + imax + imin + iabs +
                       imod1 + imod2 + imod3;
    return intArithmetic + intKernelMaths;
}

/*** Single Precision Floating point arithmetic operations */
float floatResult(int a, int b)
{
    float spA = (float)a;
    float spB = (float)b;
    float sumsp = spA + spB;
    float subsp = spA - spB;
    float prodsp = spA * spB;
    float divsp  = spA / spB;

    float sumsp2 = spA - (-spB);
    float negsp = (-spA);

    float splogRange1  = log(spA);
    float splogRange2  = log(spB);
    float splog2Range1 = log2(spA);
    float splog2Range2 = log2(spB);
    float spexp        = exp(spA);
    float spmax        = (spA >= spB)? spA : spB; // max(a, b);
    float spmin        = (spA >= spB)? spB : spA; // min(a, b);

    float spabs        = abs(spexp);
    float spceil       = ceil(spA);
    float spfloor      = floor(spA);
    float spcos        = cos(spA);
    float spsin        = sin(spA);
    float spsqrt       = sqrt(spA);

    float spscalb   = spA * 32;
    float sppow2    = 32;
    float sppow2a   = 32;

    float spArithmetic = sumsp + subsp + negsp + prodsp + divsp + sumsp2;
    float spKernelMath = splogRange1 + splogRange2 + splog2Range1 + splog2Range2 +
                          spexp + spmax + spmin + spabs + spceil + spfloor + spcos + spsin +
                          sppow2 + sppow2a + spscalb + spsqrt;
    return spArithmetic + spKernelMath;
}

/*** Double Precision Floating point arithmetic operations */
double doubleResult(int a, int b, int useTrig)
{
    double dpA = (double)a;
    double dpB = (double)b;
    double sumdp = dpA + dpB;
    double subdp = dpA - dpB;
    double proddp = dpA * dpB;
    double divdp  = dpA / dpB;

    double sumdp2 = dpA - (-dpB);
    double negdp = (-dpA);

    double dplogRange1  = log(dpA);
    double dplogRange2  = log(dpB);
    double dplog2Range1 = log2(dpA);
    double dplog2Range2 = log2(dpB);
    double dpexp        = exp(dpA);
    double dpmax        = (dpA >= dpB)? dpA : dpB; // max(a, b);
    double dpmin        = (dpA >= dpB)? dpB : dpA; // min(a, b);
    double dpabs        = abs(dpexp);
    double dpceil       = ceil(dpA);
    double dpfloor      = floor(dpA);
    double dpsqrt       = sqrt(dpA);
    double dpcos        = 0.0;
    double dpsin        = 0.0;
    double dpscalb      = dpA * 32;
    double dppow2       = 32;
    double dppow2a      = 32;

    double dpArithmetic = sumdp + subdp + negdp + proddp + divdp + sumdp2;
    double dpKernelMath = dplogRange1 + dplogRange2 + dplog2Range1 + dplog2Range2 +
                          dpexp + dpmax + dpmin + dpabs + dpceil + dpfloor + dpcos + dpsin +
                          dppow2 + dppow2a + dpscalb + dpsqrt;
    if (useTrig == 1)
    {
        dpcos           = cos(dpA);
        dpsin           = sin(dpA);
        dpKernelMath   += dpcos + dpsin;
    }

    return dpArithmetic + dpKernelMath;
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
    a[i] = i + 4;
    b[i] = i + 5;
  }

  printf("Running on DFE.\n");
  ResourceUsage(inSize, a, a, a, b, b, b, dpRes, intRes, mpRes, spRes);

  // we allow for some rounding in integer math
  int    intTol = 2;
  float  spTol  = 1e-8;
  double dpTol  = 1e-16;

  // on Max3 there's no problem compiling HW with trigonometric functions
  // in double precision but on Max4 (Maia) it doesn't compile (MaxCompiler 2013.2.2).
  int useTrigonometricFunctionsOnDFE = 1;

  for (int i = 0; i < inSize; i++) {
    int intExp  = intCpuResult(a[i],b[i]);
    float flExp = floatResult(a[i],b[i]);
    double dpExp = doubleResult(a[i],b[i], useTrigonometricFunctionsOnDFE);
    float mpExp = (float)dpExp;
    if ((intRes[i] - intExp) > intTol){
        printf("Integer output from DFE did not match CPU: %d : %d != %d\n", i, intRes[i], intExp);
        return 1;
    }
    if ((spRes[i] - flExp) > spTol){
        printf("Single precision output from DFE did not match CPU: %d : %f != %f\n", i, spRes[i], flExp);
        return 1;
    }
    if ((mpRes[i] - mpExp) > spTol){
        printf("Middle precision output from DFE did not match CPU: %d : %f != %f\n", i, mpRes[i], mpExp);
        return 1;
    }
    if ((dpRes[i] - dpExp) > dpTol){
        printf("Double precision output from DFE did not match CPU: %d : %f != %f\n", i, dpRes[i], dpExp);
        return 1;
    }

  }

  printf("Test passed!\n");
  return 0;
}
