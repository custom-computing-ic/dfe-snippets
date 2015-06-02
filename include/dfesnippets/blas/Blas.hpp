#ifndef BLAS_HPP_LTJDDIGU
#define BLAS_HPP_LTJDDIGU

#include <iostream>

namespace dfesnippets {

  namespace blas {

    using namespace std;

    typedef double value_t;
    typedef vector<value_t> vec;

    class Matrix {
      long n;
      value_t* data;

      public:
      Matrix(long _n) : n(_n) {
        data = new value_t[n * n];
      }

      ~Matrix() {
        delete [] data;
      }

      value_t& operator()(long i, long j) {
        return data[i * n + j];
      }

      vec operator* (const vec& v) const {
        vec r(n, 0);
        for (long i = 0; i < n; i++)
          for (long j = 0; j < n; j++) {
            r[i] += v[j] * data[i * n + j];
          }
        return r;
      }

      void print() {
        for (int i = 0; i < n; i ++ ) {
          for (int j = 0 ; j < n; j++)
            cout << operator()(i, j) << " ";
          std::cout << std::endl;
        }
      }

      void init_random() {
        for (long i = 0; i < n; i ++ )
          for (long j = 0 ; j < n; j++)
            //        data[i * n + j] = ((j / 48) + 1) * 10 + i % 10;
            data[i * n + j] =  j / 48 + 1;
      }

      void print_info() {
        cout << "Matrix "  << endl;
        cout << "  Dense" << endl;
        cout << "  Size (GB) " << n * n  * 8 / (1024 * 1024 * 1024) << endl;
      }

      value_t* linear_access_pointer() {
        return data;
      }

      inline int size() const {
        return n;
      }
    };

  }
}

#endif /* end of include guard: BLAS_HPP_LTJDDIGU */
