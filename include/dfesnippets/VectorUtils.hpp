#ifndef VECTORUTILS_HPP_MUVNZPCL
#define VECTORUTILS_HPP_MUVNZPCL

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
  }
}

#endif /* end of include guard: VECTORUTILS_HPP_MUVNZPCL */
