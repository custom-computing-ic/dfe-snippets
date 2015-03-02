#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <fstream>

template<typename T>
void print_vector(const std::vector<T>& v) {
  for (auto val : v)
    std::cout << val << " ";
}

template<typename T>
void print_vector_as_int(const std::vector<T>& v) {
  for (auto val : v)
    std::cout << static_cast<int>(val) << " ";
}

template<typename T>
void write_vector_to_file(const std::vector<T>& v, std::string fileName) {
  std::ofstream f(fileName);
  for (auto val : v)
    f << val << std::endl;
}

double clock_diff(std::chrono::high_resolution_clock::time_point start) {
  using namespace std::chrono;
  auto end = high_resolution_clock::now();
  double timeInSeconds = duration_cast<duration<double> >(end - start).count();
  return timeInSeconds;
}

double print_clock_diff(std::string item,
                      std::chrono::high_resolution_clock::time_point start) {
  using namespace std::chrono;
  auto end = high_resolution_clock::now();
  std::cout << item << " took ";
  double timeInSeconds = duration_cast<duration<double> >(end - start).count();
  std::cout << timeInSeconds;
  std::cout << " seconds." << std::endl;
  return timeInSeconds;
}

int align(int bytes, int k) {
  return k * (bytes / k + (bytes % k == 0 ? 0 : 1));
}

template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> in) {
  std::vector<T> flat;
  for (auto v : in)
    for (auto vv : v)
      flat.push_back(vv);
  return flat;
}

#endif
