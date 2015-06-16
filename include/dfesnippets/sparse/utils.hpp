#ifndef UTILS_HPP
#define UTILS_HPP

#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>

bool almost_equal(double a, double b);

/** Returns the smallest number greater than bytes that is multiple of k. */
int align(int bytes, int k);

/** Print the difference between now and start */
void print_clock_diff(std::string item,
                      std::chrono::high_resolution_clock::time_point start);

/** Print the difference between end and start */
void print_clock_diff(std::string item,
                      std::chrono::high_resolution_clock::time_point end,
                      std::chrono::high_resolution_clock::time_point start);

// Zip the vectors from in and merge the results;
// if the vectors are unequal, pad with 0
template <typename T>
std::vector<T> zip_flatten(std::vector<std::vector<T> > in) {
  int max_length = 0;
  for (auto v : in)
    max_length = std::max(max_length, (int)v.size());

  std::vector<T> zf;
  for (size_t i = 0; i < static_cast<size_t>(max_length); i++) {
    for (size_t j = 0; j < in.size(); j++) {
      T value = 0;
      if (i < in[j].size())
        value = in[j][i];
      zf.push_back(value);
    }
  }

  return zf;
}

template <typename T>
void print_vector(std::vector<T> in) {
  for (auto v : in)
    std::cout << v << " ";
  std::cout << std::endl;
}

// void print_char_vector(std::vector<char> in) {
//   for (auto v : in)
//     std::cout << (int)v << " ";
//   std::cout << std::endl;
// }

template <typename T>
std::vector<std::vector<T> > split_vector(std::vector<T> in, int chunk_size) {
  std::vector<std::vector<T> > chunks;
  std::vector<T> chunk;
  for (auto v : in) {
    chunk.push_back(v);
    if (chunk.size() == chunk_size) {
      chunks.push_back(chunk);
      chunk.clear();
    }
  }
  return chunks;
}

// pre: vectors must have equal length
template <typename T>
std::vector<std::vector<T> > zip_vectors(std::vector<std::vector<T> > in) {
  std::vector<std::vector<T> > zipped;

  int max_length = in[0].size();

  for (int j = 0; j < max_length; j++) {
    std::vector<T> zip;
    for (int i = 0; i < in.size(); i++) {
      zip.push_back(in[i][j]);
    }
    zipped.push_back(zip);
  }

  return zipped;
}

void print_spmv_gflops(std::string item,
                       int nnzs,
                       std::chrono::high_resolution_clock::time_point end,
                       std::chrono::high_resolution_clock::time_point start);

bool almost_equal(double a, double b) {
return std::abs(a - b) <= 1E-10 * std::abs(a);
}

/** Returns the smallest number greater than bytes that is multiple of k. */
int align(int bytes, int k) {
return  k * (bytes / k + (bytes % k == 0 ? 0 : 1));
}

/** Print the difference between now and start */
void print_clock_diff(std::string item,
              std::chrono::high_resolution_clock::time_point start) {
  using namespace std::chrono;
auto end = high_resolution_clock::now();
std::cout << item << " took ";
std::cout << duration_cast<duration<double> >(end - start).count();
std::cout << " seconds." << std::endl;
}

/** Print the difference between end and start */
void print_clock_diff(std::string item,
              std::chrono::high_resolution_clock::time_point end,
              std::chrono::high_resolution_clock::time_point start) {
  using namespace std::chrono;
std::cout << item << " took ";
std::cout << duration_cast<duration<double> >(end - start).count();
std::cout << " seconds." << std::endl;
}

void print_spmv_gflops(std::string item,
                       int nnzs,
                       std::chrono::high_resolution_clock::time_point end,
                       std::chrono::high_resolution_clock::time_point start) {
  using namespace std::chrono;
  std::cout << item << " est. GLOPS ";
  std::cout << 2.0 * nnzs / duration_cast<duration<double> >(end - start).count() / 1E9;
  std::cout << std::endl;
}


#endif
