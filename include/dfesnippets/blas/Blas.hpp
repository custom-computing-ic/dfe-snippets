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


      // converts matrix data to a strided access pattern, with given stride width (in elements)
      void convert_to_strided_access(int stride_width) {
        auto new_data = new value_t[n * n];
        if (n % stride_width != 0) {
          cerr << "Error! n must be a multiple of stride_width" << endl;
          return;
        }
        int nstrides = n / stride_width;
        int idx = 0;
        for (int i = 0; i < nstrides; i++)
          for (int row = 0; row < n; row++)
            for (int col = 0; col < stride_width; col++) {
              new_data[idx++] = data[row * n + i * stride_width + col] ;
            }
        delete [] data;
        data = new_data;
      }

      inline int size() const {
        return n;
      }
    };

  }
}

#endif /* end of include guard: BLAS_HPP_LTJDDIGU */
