#ifndef TIMING_HPP_W9D7COFX
#define TIMING_HPP_W9D7COFX

#include <chrono>
#include <string>
#include <iostream>

namespace dfesnippets {
  namespace timing {

    inline double clock_diff(
        std::chrono::high_resolution_clock::time_point start) {
      using namespace std::chrono;
      auto end = std::chrono::high_resolution_clock::now();
      return duration_cast<duration<double> >(end - start).count();
    }

    /** Print the difference between now and start */
    inline void print_clock_diff(
        std::string item,
        std::chrono::high_resolution_clock::time_point start) {
      using namespace std::chrono;
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << item << " took ";
      std::cout << duration_cast<duration<double> >(end - start).count();
      std::cout << " seconds." << std::endl;
    }

    /** Print the difference between end and start */
    inline void print_clock_diff(std::string item,
        std::chrono::high_resolution_clock::time_point end,
        std::chrono::high_resolution_clock::time_point start) {
      using namespace std::chrono;
      std::cout << item << " took ";
      std::cout << duration_cast<duration<double> >(end - start).count();
      std::cout << " seconds." << std::endl;
    }

    inline void print_spmv_gflops(std::string item,
        int nnzs,
        std::chrono::high_resolution_clock::time_point end,
        std::chrono::high_resolution_clock::time_point start) {
      using namespace std::chrono;
      std::cout << item << " est. GLOPS ";
      std::cout << 2.0 * nnzs / duration_cast<duration<double> >(end - start).count() / 1E9;
      std::cout << std::endl;
    }
  }
}

#endif /* end of include guard: TIMING_HPP_W9D7COFX */
