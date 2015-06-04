#ifndef TIMING_HPP_W9D7COFX
#define TIMING_HPP_W9D7COFX

#include <chrono>

namespace dfesnippets {
  namespace timing {

    void print_clock_diff(
        std::string item,
        std::chrono::high_resolution_clock::time_point start) {
      using namespace std::chrono;
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << item << " took ";
      std::cout << duration_cast<duration<double> >(end - start).count();
      std::cout << " seconds." << std::endl;
    }

  }
}

#endif /* end of include guard: TIMING_HPP_W9D7COFX */
