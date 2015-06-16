#include "utils.hpp"

#include <cmath>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

bool almost_equal(double a, double b) {
return std::abs(a - b) <= 1E-10 * std::abs(a);
}

/** Returns the smallest number greater than bytes that is multiple of k. */
int align(int bytes, int k) {
return  k * (bytes / k + (bytes % k == 0 ? 0 : 1));
}

/** Print the difference between now and start */
void print_clock_diff(string item,
              high_resolution_clock::time_point start) {
auto end = high_resolution_clock::now();
cout << item << " took ";
cout << duration_cast<duration<double> >(end - start).count();
cout << " seconds." << endl;
}

/** Print the difference between end and start */
void print_clock_diff(string item,
              high_resolution_clock::time_point end,
              high_resolution_clock::time_point start) {
cout << item << " took ";
cout << duration_cast<duration<double> >(end - start).count();
cout << " seconds." << endl;
}

void print_spmv_gflops(string item,
                       int nnzs,
                       high_resolution_clock::time_point end,
                       high_resolution_clock::time_point start) {
  cout << item << " est. GLOPS ";
  cout << 2.0 * nnzs / duration_cast<duration<double> >(end - start).count() / 1E9;
  cout << endl;
}


