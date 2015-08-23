#include <stdio.h>

#include <vector>
#include <iostream>

#include "ParallelCsrDecoder.h"
#include "MaxSLiCInterface.h"

int main(void)
{

  int nbursts = 4; // how many bursts we are expecting to read
  const int inputWidth = ParallelCsrDecoder_inputWidth;
  const int inSize = nbursts * inputWidth;

  std::vector<double> a(inSize);


  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
  }

  if (inputWidth != 32)
    std::cout << "Warning! This may not work for input width != 32" << std::endl;

  uint32_t allones = 0xffffffff;
  uint32_t allExceptFirst4 = 0xfffffff0;
  uint32_t first4 = !allExceptFirst4;
  std::vector<uint32_t> readmask{
    0x0000000f,
    0x000000f0,
    0x00000f00,
    0x0000f000,
    0xfffff000,
    allones,
    allones,
    allones
  };
  std::vector<uint32_t> readenable{
    1, 0, 0, 0, 0, 1, 1, 1
  };

  std::cout << "Running on DFE." << std::endl;


    //void ParallelCsrDecoder(
        //int64_t param_ticks,
        //const void *instream_a,
        //size_t instream_size_a,
        //const uint32_t *instream_readenable,
        //const uint32_t *instream_readmask,
        //double *outstream_output);

  int ticks = readmask.size();
  std::vector<double> out(ticks * inputWidth);
  ParallelCsrDecoder(
      ticks,
      &a[0],
      a.size() * sizeof(double),
      &readenable[0],
      &readmask[0],
      &out[0]);

  //for (int i = 0; i < ticks * inputWidth; ++i) {
    //if (i % inputWidth == 0)
      //std::cout << std::endl;
    //std::cout << out[i] << " ";
  //}

  for (int i = 0; i < out.size(); i++)
    std::cout << out[i] << std::endl;

  for (int i = 0; i < a.size(); i++)
    if (a[i] != out[i])
      std::cout << "Got " << out[i] << " exp " << a[i] << std::endl;

  std::cout << "Test passed!" << std::endl;
  return 0;
}
