#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#include <boost/lexical_cast.hpp>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <common.h>
#include <sparse_matrix.hpp>
#include <Fpga.h>

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

  // generate multiplicand
  vector<double> v(n);
  for (int i = 1; i <=n; i++)
    v[i - 1] = i;

  AdjustedCsrMatrix<double> original_matrix(n);
  original_matrix.load_from_csr(values, col_ind, row_ptr);
    
#ifdef DEBUG_PRINT_MATRICES
  original_matrix.print();
  original_matrix.print_dense();
#endif

  // find expected result
  vector<double> bExp = SpMV_MKL_ge((char *)path.c_str(), v);
  auto b = SpMV_DFE(original_matrix, v, numPipes, num_repeat);

  cout << "Ran SPMV " << endl;

  int errors = 0;
  for (size_t i = 0; i < b.size(); i++)
    if (!almost_equal(bExp[i], b[i])) {
      cerr << "Expected [ " << i << " ] " << bExp[i] << " got: " << b[i] << endl;
      errors++;
    }

  if (errors != 0) {
    cerr << "Errors " << errors <<endl;
    return 1;
  }

  cout << "Test passed!" << endl;
  return 0;
}
