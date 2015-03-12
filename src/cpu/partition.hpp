#ifndef PARITION_HPP
#define PARITION_HPP

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

// generate column based parititions of the original matrix
template<typename T>
std::vector<boost::numeric::ublas::compressed_matrix<T>> 
partition(boost::numeric::ublas::compressed_matrix<T> m, 
                int size) {
        using namespace boost::numeric::ublas;
        
        int nPartitions = m.size1() / size   + (m.size1() % size == 0 ? 0 : 1);
        std::vector<compressed_matrix<double>> partitions;
        for (int i = 0; i < nPartitions; ++i)
        {
                partitions.push_back(compressed_matrix<double>(m.size1(), size));
        }

        for (int i = 0; i < m.size1(); ++i)
        {
                int rowStart = m.index1_data()[i];
                int rowEnd = m.index1_data()[i + 1];

                for (int j = rowStart; j < rowEnd; ++j)
                {
                        int col = m.index2_data()[j];
                        T value = m.value_data()[j];
                        int partitionNumber = col / size;
                        partitions[partitionNumber](i, col % size) = value;
                }
        }
        return partitions;
}

#endif /* end of include guard: PARITION_HPP */
