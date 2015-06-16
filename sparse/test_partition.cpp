#include <utils.hpp>
#include <iostream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <partition.hpp>

using namespace boost::numeric::ublas;

int main () {

        int n = 4;
        int partitionSize = 2;

        CsrMatrix<double> m (n, n, n * n);
        for (int i = 0; i < n; ++i)
        {
                for (int j = 0; j < n; ++j)
                {
                        m(i, j) = i * (j + 1) + 1;
                }
        }
        auto ps = partition(m, 2);
        std::cout << "In:" << std::endl; 
        std::cout << m << std::endl;
        std::cout << "Out: " << std::endl;
        for (int i = 0; i < ps.size(); ++i)
        {
            std::cout << ps[i] << std::endl;
        }
        
        for (int i = 0; i < ps.size(); ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                    for (int k = 0; k < partitionSize; ++k)
                    {
                            double got = ps[i](j, k);
                            double exp = m(j, i * partitionSize + k); 
                            if (exp != got) {
                                    std::cout << "Error - exp: " << exp << " got: " << got << std::endl;
                                    return 1;
                            }
                    }
            }
        }
        return 0;
}
