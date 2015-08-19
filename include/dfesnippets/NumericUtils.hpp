#ifndef NUMERICUTILS_HPP_GRRJP3JZ
#define NUMERICUTILS_HPP_GRRJP3JZ

namespace dfesnippets {
  namespace numeric_utils {

    bool almost_equal(double a, double b) {
      return std::abs(a - b) <= 1E-10 * std::abs(a);
    }

    bool almost_equal(double a, double b, double tol) {
      return std::abs(a - b) <= tol * std::abs(a);
    }

    bool almost_equal(double a, double b, double rtol, double atol) {
      double diff = std::abs(a - b);
      if (diff < atol)
        return true;
      return (diff  / std::abs(a) <= rtol) && (diff / std::abs(b) <= rtol);
    }

    /** Returns the smallest number greater than bytes that is multiple of k. */
    int align(int bytes, int k) {
      return  k * (bytes / k + (bytes % k == 0 ? 0 : 1));
    }
  }
}


#endif /* end of include guard: NUMERICUTILS_HPP_GRRJP3JZ */
