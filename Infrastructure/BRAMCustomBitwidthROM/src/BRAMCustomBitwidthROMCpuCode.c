/***
    Map some 64bit wide integers to FPGA's BRAM, which is declared to store
    12bit wide integers. Add each consecutive pair of integers ... and
    check the results are numerically correct.

    Data convertion happen on the fly by means of Maxeler API.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  const int inSize  = BRAMCustomBitwidthROM_dataSize * 2;
  const int outSize = BRAMCustomBitwidthROM_dataSize;

  uint64_t *rom = malloc(sizeof(uint64_t) * inSize);
  int *expected = malloc(sizeof(int) * outSize);
  int *out = malloc(sizeof(int) * outSize);

  for(int i = 0; i < inSize; ++i)
  {
    rom[i] = i;
  }

  for(int i = 0; i < outSize; ++i)
  {
    expected[i] = rom[2*i] + rom[2*i+1];
  }

  printf("Running on DFE.\n");
  BRAMCustomBitwidthROM(outSize, out, rom);

  int status = 1;
  for (int i = 0; i < outSize; i++)
    if (out[i] != expected[i]) {
      printf("Output from DFE did not match CPU: %d : %d != %d\n", i, out[i], expected[i]);
      status = 0;
    }

  if (status)
    printf("Test passed!\n");
  else
    printf("Test failed\n");
  return 0;
}
