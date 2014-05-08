/***

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize = 32;
  const int outSize = 32 + FifoBuffer_fifoBufferCapacity;

  int *input = malloc(sizeof(int) * inSize);
  int *outShift = malloc(sizeof(int) * outSize);
  int *outOdd   = malloc(sizeof(int) * outSize);
  int *expShift = malloc(sizeof(int) * outSize);
  int *expOdd   = malloc(sizeof(int) * outSize);
  memset(outShift, 0, sizeof(int) * outSize);
  memset(outOdd,   0, sizeof(int) * outSize);
  for(int i = 0; i < inSize; ++i)
  {
    input[i] = i + 1;
  }
  int count = 0;
  for(int i = 0; i < outSize; ++i)
  {
    if (i == 2*FifoBuffer_fifoBufferCapacity)  count = 1;
    expOdd[i] = count;
    if ((i >= 2*FifoBuffer_fifoBufferCapacity) && (i % 2))  count += 2;
  }
  count = 0;
  for(int i = 0; i < outSize; ++i)
  {
    if (i == FifoBuffer_fifoBufferCapacity)  count = 1;
    if (i < FifoBuffer_fifoBufferCapacity)
        expShift[i] = 0;
    else
        expShift[i] = count++;
  }

  printf("Running on DFE.\n");

  FifoBuffer(outSize, inSize, input, outShift, outOdd);

  printf("\ncycle number:            ");
  for (int i = 0; i < outSize; i++)  printf("%2d ", i);
  printf("\ninput:                   ");
  for (int i = 0; i < inSize; i++)   printf("%2d ", input[i]);
  printf("\nexpected shifted output: ");
  for (int i = 0; i < outSize; i++)  printf("%2d ", expShift[i]);
  printf("\nshifted output from DFE: ");
  for (int i = 0; i < outSize; i++)  printf("%2d ", outShift[i]);
  printf("\nexpected odd output:     ");
  for (int i = 0; i < outSize; i++)  printf("%2d ", expOdd[i]);
  printf("\nodd output from DFE:     ");
  for (int i = 0; i < outSize; i++)  printf("%2d ", outOdd[i]);
  printf("\n\n");

  free(input);
  free(outShift);
  free(outOdd);
  free(expShift);
  free(expOdd);

  return 0;
}
