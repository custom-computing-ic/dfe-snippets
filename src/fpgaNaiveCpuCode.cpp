#include <stdio.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>

#include <boost/align/aligned_allocator.hpp>
#include <boost/lexical_cast.hpp>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#include <common.h>
#include <utils.hpp>
#include <sparse_matrix.hpp>

// #define DEBUG_PRINT_MATRICES
// #define DEBUG_PARTITIONS

using namespace std;
using namespace std::chrono;

template<class T> using aligned_vector = vector<T, boost::alignment::aligned_allocator<T, 16> >;

string check_file(char **argv) {
  string path = string(argv[2]);
  ifstream f{path};
  if (!f) {
    cout << "Error opening input file" << endl;
    exit(1);
  }
  return path;
}

void print_results(vector<double> bExp, vector<double> b) {
  cout << "CPU  = ";
  for (int i = 0; i < bExp.size(); i++)
    cout << bExp[i] << " ";
  cout << endl << "FPGA = ";
  for (int i = 0; i < b.size(); i++)
    cout << b[i] << " ";
  cout << endl;
}

int count_empty_rows(int *row_ptr, int n) {
  int prev = row_ptr[0];
  int empty_rows = 0;
  for (int i = 1; i < n + 1; i++) {
    if (prev == row_ptr[i]) {
      empty_rows++;
    }
    prev = row_ptr[i];
  }
  return empty_rows;
}

// distributes the adjusted indptr array to multiple processing elements
template <typename value_type>
tuple<vector<int>, vector<value_type>, vector<int>, vector<int> >
distribute_indptr(vector<int> adjusted_indptr,
                  vector<value_type> values,
                  int n, 
                  int npes) {

  if (n % npes != 0)
    cout << "ERROR: Cannot distribute rows evenly to PEs" << endl;

  vector<int> rows_per_pe(npes, n / npes);
  vector<int> empty_rows_per_pe(npes, 0);

  // in the case when n % npes != 0, include trailing rows in the last partition
  rows_per_pe[npes - 1] += n % npes;

  // find and merge the rows
  vector<vector<int> > rows;
  vector<vector<value_type> > row_values;
  vector<int> new_row;
  vector<value_type> new_row_values;

  int longest_row = 0;
  int k = 0;
  int values_pos = 0;
  int pe = 0;
  for (int i = 0; i < adjusted_indptr.size(); ) {
    int row_length = adjusted_indptr[i++];
    int m = i + row_length;
    new_row.push_back(row_length);
    new_row_values.push_back(0);
    //
    k++;
    if (row_length == 0) {
      // handle empty rows -- need to insert padding in the values
      // stream to avoid consuming wrong data
      //      new_row_values.push_back(0);
      empty_rows_per_pe[pe]++;
    } else {
      for (; i < m; i++) {
        new_row.push_back(adjusted_indptr[i]);
        new_row_values.push_back(values[values_pos++]);
      }
    }

    if (k == rows_per_pe[pe]) {
      rows.push_back(new_row);
      row_values.push_back(new_row_values);

      longest_row = max(longest_row, (int)new_row.size());

      new_row.clear();
      new_row_values.clear();
      k = 0;

      pe++; // move to next PE
    }
  }

#ifdef DEBUG_PARTITIONS
  cout << "Value partition: " << endl;
  for (auto v : row_values) {
    cout << "size = " << v.size() << " ";
    for (auto vv : v)
      cout << (double)vv << " ";
    cout << endl;
  }

  cout << "Indptr partition: " << endl;
  for (auto v : rows) {
    cout << "size = " << v.size() << " ";
    for (auto vv : v)
      cout << vv << " ";
    cout << endl;
  }
#endif

  vector<int> indptr_inputs_per_pipe(npes);
  vector<int> csr_inputs_per_pipe(npes);
  for (int i = 0; i < npes; i++)  {
    csr_inputs_per_pipe[i] = rows[i].size();
    indptr_inputs_per_pipe[i] = csr_inputs_per_pipe[i] - rows_per_pe[i] + empty_rows_per_pe[i];
  }

  // distribute the rows accross pes
  // add padding to make rows of equal length
  return make_tuple(zip_flatten<int>(rows),
                    zip_flatten<value_type>(row_values),
                    indptr_inputs_per_pipe,
                    csr_inputs_per_pipe);
}

template <typename value_type>
vector<double> SpMV_DFE(AdjustedCsrMatrix<value_type> m,
                        vector<double> v,
                        int num_pipes,
                        int num_repeat) {

  int empty_rows = m.get_empty_rows();

  auto start_time = high_resolution_clock::now();

  // -- Distribute values to PEs
  cout << "Distributing data to PEs..." << endl;
  auto res = distribute_indptr<value_type>(m.adjusted_ind, m.values, m.n, num_pipes);
  auto adjusted_indptr = get<0>(res);
  auto values = get<1>(res);
  auto pipe_input_count = get<2>(res);
  print_clock_diff("Done: ", start_time);

  // -- dimensions in bytes for the encoding streams
  int index_size = align(adjusted_indptr.size(), 384);
  int value_size = align(values.size() * sizeof(value_type), 384);

  // stream size must be multiple of 16 bytes
  // padding bytes are ignored in the actual kernel
  int nnzs_bytes = m.nnzs * sizeof(value_type);
  int indptr_size  = align(nnzs_bytes, 384);
  int row_ptr_size = align(m.n * sizeof(int), 384);
  int adjusted_indptr_size = align(adjusted_indptr.size() * sizeof(int), 384);

  // --- Running whole SpMV design
  cout << "      Running on DFE." << endl;
  cout << "          num_pipes = " << num_pipes << endl;
  cout << "                  n = " << m.n << endl;
  cout << "               nnzs = " << m.nnzs << endl;
  cout << "         empty rows = " << empty_rows << endl;
  cout << "             ticks  = " << values.size() / num_pipes << endl;
  cout << "       total cycles = " << m.nnzs + empty_rows << endl;
  cout << "         value_size = " << value_size << " bytes " << endl;
  cout << "        indptr_size = " << indptr_size << " bytes " << endl;
  cout << "adjusted_indptr_size = " << adjusted_indptr_size << " bytes" << endl;

  start_time = high_resolution_clock::now();

  SpmvBase_writeDRAM(adjusted_indptr_size,
                      0,
                      (uint8_t *)&adjusted_indptr[0]);
  SpmvBase_writeDRAM(value_size,
                      adjusted_indptr_size,
                      (uint8_t*)&values[0]);
  print_clock_diff("Writing to DRAM ", start_time);

  start_time = high_resolution_clock::now();

  SpmvBase_setBRAMs(
                     &v[0],
                    &v[0],
                    &v[0],
                    &v[0]);

  print_clock_diff("Writing to BRAMs ", start_time);

  vector<double> b(m.n * 2, 0);
  start_time = high_resolution_clock::now();
  
  if (values.size() % num_pipes != 0)
    cout << "ERROR! This cannot happen!!!" << endl;
  if (adjusted_indptr.size() % num_pipes != 0)
    cout << "ERROR! This cannot happen!!!" << endl;

  vector<int64_t> indptr_size_per_pipe;
  for (auto val : get<2>(res))
    indptr_size_per_pipe.push_back(val);

  vector<int64_t> csr_size_per_pipe;
  for (auto val : get<3>(res))
    csr_size_per_pipe.push_back(val);

  cout << "Indptr_size_per_pipe" << endl;
  print_vector(indptr_size_per_pipe);
  for (int i = 0; i < num_repeat; i++)
  {
      SpmvBase(
                adjusted_indptr_size,
                m.n,
                values.size() / num_pipes,
                value_size,
                &csr_size_per_pipe[0],
                &indptr_size_per_pipe[0],
                values.size() / num_pipes,
                &b[0]
            );
  }

  auto end_time = high_resolution_clock::now();

  cout << "\nDone " << num_repeat << "runs of SpMV. _Adjust run times accordingly_\n";

  std::string message = std::string("SpMV ");

  print_clock_diff(message, end_time, start_time);
  print_spmv_gflops(message, m.nnzs, end_time, start_time);

  vector<pair<double, double> > r;
  for (int i = 0; i < b.size(); i+=2) {
    r.push_back(make_pair(b[i], b[i + 1]));
  }
  
  sort(r.begin(), r.end());
  vector<double> r2;
  for (auto p : r) {
    r2.push_back(p.second);
  }

  return r2;
}

int main(int argc, char** argv) {

  cout << "Program arguments:" << endl;
  for (int i = 0; i < argc; i++)
    cout << "   " << argv[i] << endl;

  string path = check_file(argv);
  int num_repeat = boost::lexical_cast<int>(argv[3]);

  // -- Design Parameters
  int fpL = SpmvBase_fpL;  // adder latency
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

  cout << "HERE" << endl;
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
  for (int i = 0; i < b.size(); i++)
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
