/***
    Sample hardware design: provides C API to the hardware kernel.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

// just in case we want to compile this using C++ compiler
/*
#ifdef __cplusplus
extern "C"
#endif
int hardwareAPI(int x);
#ifdef __cplusplus
}
#endif
*/

int hardwareAPI(int x)
{
  const int inSize = 384;

  int *a = (int*)malloc(sizeof(int) * inSize);
  int *b = (int*)malloc(sizeof(int) * inSize);
  int *expected = (int*)malloc(sizeof(int) * inSize);
  int *out = (int*)malloc(sizeof(int) * inSize);
  memset(out, 0, sizeof(int) * inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1 + x;
    b[i] = i - 1 + x;
    expected[i] = 2 * i + 2*x;
  }

  printf("Running on DFE.\n");
  LinkingThirdPartyLibrary(inSize, a, b, out);


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
