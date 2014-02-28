/***
    This is a simple demo project that you can copy to get started.
    Comments blocks starting with '***' and subsequent non-empty lines
    are automatically added to this project's wiki page.
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
  int *out = malloc(sizeof(int) * inSize);
  memset(out, 0, sizeof(int) * inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
    b[i] = i - 1;
    expected[i] = 2 * i;
  }

  printf("Running on DFE.\n");
  %%%ProjectName%%%(inSize, a, b, out);


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
