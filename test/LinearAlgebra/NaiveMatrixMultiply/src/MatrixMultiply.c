/***
    This project performs a naive matrix multiplication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

// two 24 x 24 matrices = 576 ints = 6 * 384 bytes; this constant is
// defined in the Manager, to make sure the CPU and DFE are in sync
const int inSize = MatrixMultiply_matrixSize;

void print_matrix(int *mat, int size) {
  for(int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j)
      printf("%d ", mat[i * inSize + j]);
    printf("\n");
  }
}


void matrix_mult(int *a, int *b, int *res) {
  for(int i = 0; i < inSize; ++i)
    for (int j = 0; j < inSize; ++j)
      for (int k = 0; k < inSize; k++)
        res[i * inSize + j]  += a[i * inSize + k] * b[k * inSize + j ];
}


void transpose_matrix(int *mat, int size) {

  for (int i = 0 ; i < size; i++)
    for (int j = i; j < size; j++) {
      int t = mat[i * size + j];
      mat[i * size + j] = mat[j * size + i];
      mat[j * size + i] = t;
    }
}


int main(void)
{
  int *a = malloc (sizeof(int) * inSize * inSize);
  int *b = malloc (sizeof(int) * inSize * inSize);
  int *expected = malloc (sizeof(int) * inSize * inSize);
  int *out = malloc (sizeof(int) * inSize * inSize);

  memset(a, 0, sizeof(int) * inSize * inSize);
  memset(b, 0, sizeof(int) * inSize * inSize);

  memset(out, 0, sizeof(int) * inSize * inSize);
  memset(expected, 0, sizeof(int) * inSize  * inSize);

  // if you want deterministic random numbers, comment out the line below
  srand(time(NULL));

  for(int i = 0; i < inSize; ++i) {
    for (int j = 0; j < inSize; ++j) {
      a[i * inSize + j] = rand() % 100;
      b[i * inSize + j] = rand() % 100;
    }
  }

  printf("Matrix A:\n");
  print_matrix(a, inSize);

  printf("\nMatrix B:\n");
  print_matrix(b, inSize);
  matrix_mult(a, b, expected);

  printf("\nCPU result:\n");
  print_matrix(expected, inSize);

  // we need to transpose the matrix before streaming it to the DFE
  transpose_matrix(b, inSize);
  printf("\nMatrix B (transposed):\n");
  print_matrix(b, inSize);

  // we need to queue on b matrix for every line in the a matrix this
  // can be achieved in a less wasteful manner by using a custom
  // memory command stream (therefore only streaming the b data once
  // over slow PCIe)
  int *large_b = malloc(sizeof(int) * inSize * inSize * inSize);

  for (int k = 0; k < inSize; ++k)
    for(int i = 0; i < inSize; ++i)
      for (int j = 0; j < inSize; ++j)
        large_b[k * inSize * inSize + i * inSize + j] = b[i * inSize + j];

  // we also need to queue repeated rows of a
  int *large_a = malloc(sizeof(int) * inSize * inSize * inSize);
  for(int i = 0; i < inSize; ++i)
    for (int k = 0; k < inSize; ++k)
      for (int j = 0; j < inSize; ++j)
        large_a[i * inSize * inSize + k * inSize + j] = a[i * inSize + j];

  printf("\nRunning on DFE.\n");
  MatrixMultiply(inSize * inSize * inSize, large_a, large_b, out);
  print_matrix(out, inSize);

  /***
      Note that you should always test the output of your DFE
      design against a CPU version to ensure correctness.
  */


  for (int i = 0; i < inSize; i++)
    for (int j = 0; j < inSize; j++)
      if (out[i * inSize + j] != expected[i * inSize + j]) {
        printf("Output from DFE did not match CPU: %d : %d != %d\n",
               i, out[i * inSize + j], expected[i * inSize + j]);
        return 1;
      }

  printf("\nTest passed!\n");
  return 0;
}
