#ifndef SCHEDULING_HPP
#define SCHEDULING_HPP

#include <vector>
#include <tuple>

// distributes the adjusted indptr array to multiple processing elements
template <typename value_type>
std::tuple<std::vector<int>, std::vector<value_type>, std::vector<int>, std::vector<int> >
distribute_indptr(std::vector<int> adjusted_indptr,
                  std::vector<value_type> values,
                  int n, 
                  int npes) {
  using namespace std;
  if (n % npes != 0)
    cout << "ERROR: Cannot distribute rows evenly to PEs" << endl;

  std::vector<int> rows_per_pe(npes, n / npes);
  std::vector<int> empty_rows_per_pe(npes, 0);

  // in the case when n % npes != 0, include trailing rows in the last partition
  rows_per_pe[npes - 1] += n % npes;

  // find and merge the rows
  std::vector<std::vector<int> > rows;
  std::vector<std::vector<value_type> > row_values;
  std::vector<int> new_row;
  std::vector<value_type> new_row_values;

  int longest_row = 0;
  int k = 0;
  int values_pos = 0;
  int pe = 0;
  for (size_t i = 0; i < adjusted_indptr.size(); ) {
    int row_length = adjusted_indptr[i++];
    size_t m = i + row_length;
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

  std::vector<int> indptr_inputs_per_pipe(npes);
  std::vector<int> csr_inputs_per_pipe(npes);
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



#endif /* end of include guard: SCHEDULING_HPP */
