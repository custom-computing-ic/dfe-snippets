#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <vector>
#include <iostream>
#include <iomanip>

#include <utils.hpp>


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
    for (int i = 0; i < adjusted_ind.size();) {
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
    for (int i = 0; i < adjusted_ind.size();) {
      int row_length = adjusted_ind[i++];
      int m = row_length + i;
      for (; i < m; i++) {
        result[row] += x[adjusted_ind[i]] * values[pos++];
      }
      row++;
    }

    return result;
  }

  void load_from_csr(double* values, int* col_ind, int* row_ptr) {
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
