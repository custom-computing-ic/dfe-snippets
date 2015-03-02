#ifndef CPU_H
#define CPU_H

#include <vector>

std::pair<std::vector<float>, std::vector<float>>
  runCPU(const std::vector<float>& a, 
         const std::vector<float>& b) {
  std::vector<float> sum(a.size(), 0), diff(a.size(), 0);
  //#pragma omp parallel for
  for (size_t i = 0; i < a.size();i++) {
    sum[i] = a[i] + b[i];
    diff[i] = a[i] - b[i];
  }

  return make_pair(sum, diff);
}


#endif
