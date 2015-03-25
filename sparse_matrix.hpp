#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include "mkl_spblas.h"
#include <utils.hpp>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

template<typename T = double> using CsrMatrix = boost::numeric::ublas::compressed_matrix<
        T,
        boost::numeric::ublas::row_major,
        0,
        std::vector<unsigned int>
        >;


std::vector<double> SpMV_MKL_ge(char *path,
                std::vector<double> v) {
        int n, nnzs;
        double* values;
        int *col_ind, *row_ptr;

        read_ge_mm_csr(path, &n, &nnzs, &col_ind, &row_ptr, &values);

        std::vector<double> r(n);
        char tr = 'N';
        auto start_time = std::chrono::high_resolution_clock::now();
        mkl_dcsrgemv(&tr, &n, values, row_ptr, col_ind, &v[0], &r[0]);
        auto end_time = std::chrono::high_resolution_clock::now();
        print_clock_diff("SpMV (CPU)", end_time, start_time);
        print_spmv_gflops("SpMV (CPU)", nnzs, end_time, start_time);
        return r;
}

std::vector<double> SpMV_MKL_unsym(char *path,
                std::vector<double> v) {
        FILE *f = fopen(path, "r");

        int n, nnzs;
        double* values;
        int *col_ind, *row_ptr;
        read_system_matrix_unsym_csr(f, &n, &nnzs, &col_ind, &row_ptr, &values);
        printf("n: %d, nnzs: %d\n", n, nnzs);

        std::vector<double> r(n);
        char tr = 'N';
        mkl_dcsrgemv(&tr, &n, values, row_ptr, col_ind, &v[0], &r[0]);

        fclose(f);
        return r;
}

std::vector<double> SpMV_MKL_sym(char *path,
                std::vector<double> v) {

        FILE *f = fopen(path, "r");

        int n, nnzs;
        double* values;
        int *col_ind, *row_ptr;
        read_system_matrix_sym_csr(f, &n, &nnzs, &col_ind, &row_ptr, &values);
        printf("n: %d, nnzs: %d\n", n, nnzs);

        std::vector<double> r(n);
        char tr = 'l';
        mkl_dcsrsymv(&tr, &n, values, row_ptr, col_ind, &v[0], &r[0]);

        fclose(f);
        return r;
}

std::vector<double> SpMV_CPU(char *path, std::vector<double> v, bool symmetric) {
  std::vector<double> r2 = SpMV_MKL_unsym(path, v);
  if (symmetric) {
    std::vector<double> r = SpMV_MKL_sym(path, v);
    std::cout << "Checking sym/unsymmetric match...";
    for (size_t i = 0; i < v.size(); i++) {
      if (!almost_equal(r[i], r2[i])) {
        printf(" r[%ld] == %f != r[%ld] == %f\n", i, r[i], i, r2[i]);
        exit(1);
      }
    }
    std::cout << "done!" << std::endl;
  }
  return r2;
}


// converts standard CSR to a format where the row_ptr and
// col_ind are merged into one array which contains entries for each
// row, first the length of the row followed by its elements
template<class value_type>
class AdjustedCsrMatrix {

  int empty_rows;

public:
  int n, nnzs;
  std::vector<value_type> values;
  std::vector<int> adjusted_ind;

  AdjustedCsrMatrix(int n_) : n(n_) {
    this->nnzs = 0;
    this->empty_rows = 0;
  }

  void add_row(std::vector<value_type> row_value, std::vector<int> row_col_ind) {
    values.insert(values.end(), row_value.begin(), row_value.end());
    if (row_col_ind.size() == 0) {
      empty_rows++;
    }
    adjusted_ind.push_back(row_col_ind.size());
    adjusted_ind.insert(adjusted_ind.end(), row_col_ind.begin(), row_col_ind.end());
    this->nnzs += row_value.size();
  }

  int get_empty_rows() {
    return empty_rows;
  }

  void print() {
    std::cout << "ACSR Matrix n = " << n << " nnzs = " << nnzs << std::endl;
    std::cout << "  Values (size = " << values.size() << "): ";
    print_vector(values);
    std::cout << "  Adjusted_indptr (size = " << adjusted_ind.size() << "): ";
    print_vector(adjusted_ind);
  }

  void print_dense() {
    int pos = 0;
    for (size_t i = 0; i < adjusted_ind.size();) {
      int row_length = adjusted_ind[i++];
      int seen = 0;
      for (int j = 0; j < n; j++) {
        if (seen == row_length) {
          std::cout << " --  ";
          continue;
        }
        if (j == adjusted_ind[i]) {
          i++;
          seen++;
          std::cout << std::setprecision(2) << std::fixed << static_cast<double>(values[pos++]) << " ";
        } else
          std::cout << " --  ";
      }
      std::cout << std::endl;
    }
  }

  std::vector<value_type> dot(std::vector<value_type> x) {
    std::vector<value_type> result(n, 0);

    int pos = 0;
    int row = 0;
    for (size_t i = 0; i < adjusted_ind.size();) {
      int row_length = adjusted_ind[i++];
      int m = row_length + i;
      for (; i < m; i++) {
        result[row] += x[adjusted_ind[i]] * values[pos++];
      }
      row++;
    }

    return result;
  }

  void load_from_csr(double* values, unsigned int* col_ind, unsigned int* row_ptr) {
    std::vector<value_type> row_values;
    std::vector<int> n_col_ind;

    int pos = 0;
    for (int i = 0; i < n; i++) {
      int rowLength = row_ptr[i + 1] - row_ptr[i];
      for (int j = 0; j < rowLength; j++) {
        double value = values[pos];
        row_values.push_back(value);
        n_col_ind.push_back(col_ind[pos]);
        pos++;
      }
      add_row(row_values, n_col_ind);
      row_values.clear();
      n_col_ind.clear();
    }
  }
};

#endif
