#include <stdio.h>

#include <vector>
#include <iostream>
#include <bitset>
#include <tuple>
#include <algorithm>

#include "ParallelCsrDecoder.h"
#include "MaxSLiCInterface.h"

template<typename value_type>
std::tuple<std::vector<value_type>, int> test_binary2(
    const std::vector<value_type>& data,
    const std::vector<uint32_t>& reads,
    int bufferWidth) {

  std::cout << "All binary" << std::endl;
  int crtPos = 0;
  std::vector<uint32_t> res;
  std::vector<value_type> result;
  int pos = 0;
  int cycles = 0;
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
      cycles++;
    }
  }

  //for (auto a : res)
    //std::cout << std::bitset<32>(a) << std::endl;
  return std::make_tuple(result, cycles);
}

int main() {

  const int inputWidth = ParallelCsrDecoder_inputWidth;
  std::vector<uint32_t> length {
    4, 4, 4, 4, 16, 32, 32, 4, 4, 4, 4, 80
  };
  int inSize = std::accumulate(length.begin(), length.end(), 0);
  std::vector<double> a(inSize);
  for(int i = 0; i < inSize; ++i) {
    a[i] = i + 1;
  }
  auto texp = test_binary2<double>(a, length, inputWidth);
  auto exp = std::get<0>(texp);
  int expCycles = std::get<1>(texp);

  std::cout << "Running on DFE (" << expCycles << " cycles )" << std::endl;
  int ticks = expCycles;
  std::vector<double> out(ticks * inputWidth, 10);
  ParallelCsrDecoder_ParallelCsrWrite(
      a.size() * sizeof(double),
      0,
      (uint8_t *)&a[0]);

  ParallelCsrDecoder(
      ticks,
      &length[0],
      length.size() * sizeof(uint32_t),
      &out[0],
      out.size() * sizeof(double),
      0,
      a.size() * sizeof(double));

  int status = 0;
  for (size_t i = 0; i < exp.size(); i++)
    if (exp[i] != out[i]) {
      std::cout << i << ": got " << out[i] << " exp " << exp[i] << std::endl;
      status |= 1;
    }

  if (status == 0)
    std::cout << "Test passed!" << std::endl;
  return status;
}
