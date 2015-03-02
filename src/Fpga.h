#ifndef FPGA_H
#define FPGA_H

#include "Maxfiles.h"

template<typename T>
int write(int size, int start, std::vector<T> data) {
  SpmvBase_writeDRAM(size, start, (uint8_t *)&data[0]);
  return start + size;
}

void writeDRAM(const std::vector<float> a) {
  std::cout << "Writing data to DRAM... " << std::endl;

  double MB = 1024 * 1024;
  int sizeBytes = a.size() * sizeof(float);
  std::cout << "    (MB) = " << a.size() << std::endl;

  Eval_write(sizeBytes, 0, a);
}

std::pair<std::vector<float>, std::vector<float>>
  runFPGA(const std::vector<float>& a, 
          const std::vector<float>& b) {

  std::vector<float> sum(a.size(), 0), diff(a.size(), 0);
  SpmvBase(a.size(),
           &b[0],
           &sum[0]);

  SpmvBAse_readDRAM(diff.size() * sizeof(float),
                a.size() * sizeof(float),
                (uint8_t *)&diff[0]);

  return make_pair(sum, diff);
}


#endif
