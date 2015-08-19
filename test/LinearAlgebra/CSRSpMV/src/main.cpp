#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <dfesnippets/sparse/common.hpp>
#include <dfesnippets/sparse/sparse_matrix.hpp>
#include <dfesnippets/sparse/partition.hpp>

#include "fpga.hpp"
// #define DEBUG_PRINT_MATRICES
// #define DEBUG_PARTITIONS

using namespace std;

string check_file(char **argv) {
  string path{argv[2]};
  ifstream f{path};
  if (!f) {
    cout << "Error opening input file" << endl;
    exit(1);
  }
  return path;
}

int main(int argc, char** argv) {

  cout << "Program arguments:" << endl;
  for (int i = 0; i < argc; i++)
    cout << "   " << argv[i] << endl;

  string path = check_file(argv);
  int num_repeat = boost::lexical_cast<int>(argv[3]);

  // -- Design Parameters
  int numPipes = SpmvBase_numPipes;

  // -- Matrix Parameters
  int n, nnzs;
  double* values;
  int *col_ind, *row_ptr;

  read_ge_mm_csr((char *)path.c_str(), &n, &nnzs, &col_ind, &row_ptr, &values);

  // adjust from 1 indexed CSR (used by MKL) to 0 indexed CSR
  for (int i = 0; i < nnzs; i++)
    col_ind[i]--;

  using namespace boost::numeric;
  // generate multiplicand
  vector<double> v(n);
  ublas::vector<double> vu(n);
  for (int i = 1; i <=n; i++) {
    v[i - 1] = i;
    vu(i - 1) = i;
  }

  // -- load the CSR matrix
  CsrMatrix<> inMatrix(n, n);

  for (int i = 0; i < n; ++i)
  {
          int rowStart = row_ptr[i] - 1;
          int rowEnd = row_ptr[i + 1] - 1;
          for (int j = rowStart; j < rowEnd; ++j)
          {
                  inMatrix(i, col_ind[j]) = values[j];
          }
  }


  auto res = ublas::prod(inMatrix, vu);
  vector<double> bExp = SpMV_MKL_ge((char *)path.c_str(), v);

  int partitionSize = min(SpmvBase_vectorCacheSize, n);
  vector<CsrMatrix<double>> partitions = partition(inMatrix, partitionSize);
  vector<double> dfe_res(n, 0);

  AdjustedCsrMatrix<double> full_original_matrix(n);
  full_original_matrix.load_from_csr(
      &inMatrix.value_data()[0],
      &inMatrix.index2_data()[0],
      &inMatrix.index1_data()[0]);

#ifdef DEBUG_PRINT_MATRICES
  cout << "Full original matrix " << endl;
  full_original_matrix.print_dense();
  cout << "Vector: " << endl;
  print_vector(v);
#endif

  int offset = 0;
  for (size_t i = 0; i < partitions.size(); ++i) {
    auto p = partitions[i];
    AdjustedCsrMatrix<double> original_matrix(n);
    original_matrix.load_from_csr(
        &p.value_data()[0],
        &p.index2_data()[0],
        &p.index1_data()[0]
        );

#ifdef DEBUG_PRINT_MATRICES
    original_matrix.print();
    original_matrix.print_dense();
#endif

    // find expected result
    vector<double> vblock(v.begin() + offset, v.begin() + offset + partitionSize);
    auto b = SpMV_DFE(original_matrix, vblock, numPipes, num_repeat);
    for (int j = 0; j < n; ++j)
    {
      dfe_res[j] += b[j];
    }
    offset += partitionSize;
  }

  cout << "Checking ublas " << endl;
  for (int i = 0; i < n; ++i) {
    if (!dfesnippets::numeric_utils::almost_equal(res(i), bExp[i])) {
      cerr << "Expected " << bExp[i] << " got: " << res(i) << endl;
      exit(1);
    }
  }
  cout << "Ran SPMV " << endl;

  int errors = 0;
  for (size_t i = 0; i < dfe_res.size(); i++)
    if (!dfesnippets::numeric_utils::almost_equal(bExp[i], dfe_res[i])) {
      cerr << "Expected [ " << i << " ] " << bExp[i] << " got: " << dfe_res[i] << endl;
      errors++;
    }

  if (errors != 0) {
    cerr << "Errors " << errors <<endl;
    return 1;
  }

  cout << "Test passed!" << endl;
  return 0;
}
