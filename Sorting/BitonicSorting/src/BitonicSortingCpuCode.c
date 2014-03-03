/***
    Sorts nVectors vectors of networkWidth size each.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int cmp(const void *e1, const void *e2) {
  float f1 = *((float *)e1);
  float f2 = *((float *)e2);
  if (f1 > f2) return 1;
  if (f1 < f2) return -1;
  return 0;
}

void print_vec(float *vec, int size) {
  for(int i = 0; i < size - 1; ++i)
    printf("%f ", vec[i]);
  printf("%f\n", vec[size - 1]);
}

int main(void) {

  int networkWidth = BitonicSorting_networkWidth;
  int nVectors = 1;
  int inSize = networkWidth * nVectors;

  float *in_array = malloc(sizeof(float) * inSize);
  float *expected = malloc(sizeof(float) * inSize);
  float *out_array = malloc(sizeof(float) * inSize);

  for(int i = 0; i < inSize; ++i)
    expected[i] = in_array[i] = (float)rand() / (float)rand();

  for (int  i = 0; i < nVectors; i++)
    qsort(expected + i * networkWidth, networkWidth, sizeof(float), cmp);

  printf("\nRunning on DFE.\n");
  BitonicSorting(nVectors, in_array, out_array);

  printf("Input:\n\t");
  print_vec(in_array, inSize);

  printf("\nExpected (sorted):\n\t");
  print_vec(expected, inSize);

  printf("\nOutput (sorted):\n\t");
  print_vec(expected, inSize);

  for (int i = 0; i < inSize; i++)
    if (out_array[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %i : %f != %f\n",
             i, out_array[i], expected[i]);
      return 1;
    }

  printf("Test passed!\n");
  return 0;
}
