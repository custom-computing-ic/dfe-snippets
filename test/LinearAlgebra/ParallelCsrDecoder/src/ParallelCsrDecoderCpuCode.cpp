#include <stdio.h>

#include <vector>
#include <iostream>
#include <bitset>

#include "ParallelCsrDecoder.h"
#include "MaxSLiCInterface.h"

template<typename value_type>
std::vector<value_type> test_binary2(
    const std::vector<value_type>& data,
    const std::vector<uint32_t>& reads,
    int bufferWidth) {

  std::cout << "All binary" << std::endl;
  int crtPos = 0;
  std::vector<uint32_t> res;
  std::vector<value_type> result;
  int pos = 0;
  for (auto toread : reads) {
    while (toread > 0) {
      int canread = std::min((uint32_t)(bufferWidth - crtPos), toread);
      uint32_t readmask = ((1l << canread) - 1) << crtPos;
      res.push_back(readmask);

      for (int i = 0; i < crtPos; i++)
        result.push_back(0);
      for (int i = crtPos; i < crtPos + canread; i++)
        result.push_back(data[pos++]);
      for (int i = crtPos + canread; i < bufferWidth; i++)
        result.push_back(0);

      crtPos = (crtPos + canread) % bufferWidth;
      toread -= canread;
    }
  }

  //for (auto a : res)
    //std::cout << std::bitset<32>(a) << std::endl;
  return result;
}

int main() {

  const int inputWidth = ParallelCsrDecoder_inputWidth;
  int nbursts = 4; // how many bursts we are expecting to read
  const int inSize = nbursts * inputWidth;
  std::vector<uint32_t> length{
    4, 4, 4, 4, 16, 32, 32, 4, 4, 4, 4, 16,
  };
  std::vector<double> a(inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
  }
  auto exp = test_binary2<double>(a, length, inputWidth);

  if (inputWidth != 32)
    std::cout << "Warning! This may not work for input width != 32" << std::endl;

  std::cout << "Running on DFE." << std::endl;
  int ticks = length.size();
  std::vector<double> out(ticks * inputWidth, 10);
  ParallelCsrDecoder(
      ticks,
      &a[0],
      a.size() * sizeof(double),
      &length[0],
      length.size() * sizeof(uint32_t),
      &out[0]);

  int status = 0;
  for (size_t i = 0; i < a.size(); i++)
    if (exp[i] != out[i]) {
      std::cout << "Got " << out[i] << " exp " << exp[i] << std::endl;
      status |= 1;
    }

  if (status == 0)
    std::cout << "Test passed!" << std::endl;
  return status;
}
