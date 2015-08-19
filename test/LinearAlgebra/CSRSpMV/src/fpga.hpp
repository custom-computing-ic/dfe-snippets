#ifndef FPGA_H
#define FPGA_H

#include <vector>
#include <iostream>
#include <chrono>
#include "Maxfiles.h"
#include <dfesnippets/sparse/sparse_matrix.hpp>
#include <dfesnippets/Timing.hpp>

#include "scheduling.hpp"

template <typename value_type>
std::vector<double> SpMV_DFE(AdjustedCsrMatrix<value_type> m,
                        std::vector<double> v,
                        int num_pipes,
                        int num_repeat) {
  using namespace std;
  using namespace std::chrono;

  int empty_rows = m.get_empty_rows();

  auto start_time = high_resolution_clock::now();

  // -- Distribute values to PEs
  cout << "Distributing data to PEs..." << endl;
  auto res = distribute_indptr<value_type>(m.adjusted_ind, m.values, m.n, num_pipes);
  auto adjusted_indptr = get<0>(res);
  auto values = get<1>(res);
  auto pipe_input_count = get<2>(res);
  dfesnippets::timing::print_clock_diff("Done: ", start_time);

  // -- dimensions in bytes for the encoding streams
  int value_size = dfesnippets::numeric_utils::align(values.size() * sizeof(value_type), 384);
  int adjusted_indptr_size = dfesnippets::numeric_utils::align(adjusted_indptr.size() * sizeof(int), 384);

  // --- Running whole SpMV design
  cout << "      Running on DFE." << endl;
  cout << "          num_pipes = " << num_pipes << endl;
  cout << "                  n = " << m.n << endl;
  cout << "               nnzs = " << m.nnzs << endl;
  cout << "         empty rows = " << empty_rows << endl;
  cout << "             ticks  = " << values.size() / num_pipes << endl;
  cout << "       total cycles = " << m.nnzs + empty_rows << endl;
  cout << "         value_size = " << value_size << " bytes " << endl;
  cout << "adjusted_indptr_size = " << adjusted_indptr_size << " bytes" << endl;

  start_time = high_resolution_clock::now();

  SpmvBase_writeDRAM(adjusted_indptr_size,
                      0,
                      (uint8_t *)&adjusted_indptr[0]);
  SpmvBase_writeDRAM(value_size,
                      adjusted_indptr_size,
                      (uint8_t*)&values[0]);
  dfesnippets::timing::print_clock_diff("Writing to DRAM ", start_time);

  start_time = high_resolution_clock::now();

  SpmvBase_setBRAMs(
                    &v[0],
                    &v[0]);

  dfesnippets::timing::print_clock_diff("Writing to BRAMs ", start_time);

  std::vector<double> b(m.n * 2, 0);
  start_time = high_resolution_clock::now();

  if (values.size() % num_pipes != 0)
    cout << "ERROR! This cannot happen!!!" << endl;
  if (adjusted_indptr.size() % num_pipes != 0)
    cout << "ERROR! This cannot happen!!!" << endl;

  std::vector<int64_t> indptr_size_per_pipe;
  for (auto val : get<2>(res))
    indptr_size_per_pipe.push_back(val);

  std::vector<int64_t> csr_size_per_pipe;
  for (auto val : get<3>(res))
    csr_size_per_pipe.push_back(val);

  cout << "Indptr_size_per_pipe" << endl;
  dfesnippets::vectorutils::print_vector(indptr_size_per_pipe);
  for (int i = 0; i < num_repeat; i++)
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

  auto end_time = high_resolution_clock::now();

  cout << "Done " << num_repeat << "runs of SpMV. _Adjust run times accordingly_\n";

  dfesnippets::timing::print_clock_diff("SpMV ", end_time, start_time);
  dfesnippets::timing::print_spmv_gflops("SpMV ", m.nnzs, end_time, start_time);

  std::vector<pair<double, double> > r;
  for (size_t i = 0; i < b.size(); i+=2) {
    r.push_back(make_pair(b[i], b[i + 1]));
  }

  sort(r.begin(), r.end());
  std::vector<double> r2;
  for (auto p : r) {
    r2.push_back(p.second);
  }

  return r2;
}

#endif
