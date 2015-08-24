#ifndef VECTORUTILS_HPP_MUVNZPCL
#define VECTORUTILS_HPP_MUVNZPCL

#include <vector>
#include <iostream>

namespace dfesnippets {
  namespace vectorutils {
    // copy a vector n times; e.g.
    // v = {1, 2}
    // ncopy(v, 3) == {1, 2, 1, 2, 1, 2}
    template<typename T> std::vector<T> ncopy(std::vector<T> v, int ncopies) {
      auto size = v.size();
      std::vector<double> copyv(size * ncopies, 1);
      for (int j = 0; j < ncopies; j++)
        for (int i = 0; i < size; ++i)
          copyv[j * size + i] = v[i];
      return copyv;
    }

    template <typename T>
      void print_vector(std::vector<T> in) {
        for (auto v : in)
          std::cout << v << " ";
        std::cout << std::endl;
      }

    // Zip the vectors from in and merge the results;
    // if the vectors are unequal, pad with 0
    template <typename T>
      std::vector<T> zip_flatten(std::vector<std::vector<T> > in) {
        int max_length = 0;
        for (auto v : in)
          max_length = std::max(max_length, (int)v.size());

        std::vector<T> zf;
        for (size_t i = 0; i < static_cast<size_t>(max_length); i++) {
          for (size_t j = 0; j < in.size(); j++) {
            T value = 0;
            if (i < in[j].size())
              value = in[j][i];
            zf.push_back(value);
          }
        }

        return zf;
      }

    template <typename T>
      std::vector<std::vector<T> > split_vector(std::vector<T> in, int chunk_size) {
        std::vector<std::vector<T> > chunks;
        std::vector<T> chunk;
        for (auto v : in) {
          chunk.push_back(v);
          if (chunk.size() == chunk_size) {
            chunks.push_back(chunk);
            chunk.clear();
          }
        }
        return chunks;
      }

    // pre: vectors must have equal length
    template <typename T>
      std::vector<std::vector<T> > zip_vectors(std::vector<std::vector<T> > in) {
        std::vector<std::vector<T> > zipped;

        int max_length = in[0].size();

        for (int j = 0; j < max_length; j++) {
          std::vector<T> zip;
          for (int i = 0; i < in.size(); i++) {
            zip.push_back(in[i][j]);
          }
          zipped.push_back(zip);
        }

        return zipped;
      }
  }
}

#endif /* end of include guard: VECTORUTILS_HPP_MUVNZPCL */
