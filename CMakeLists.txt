cmake_minimum_required(VERSION 2.6)

project("dfe-snippets")

enable_testing()

if (DEFINED ENV{MAXCOMPILERDIR} AND DEFINED ENV{MAXELEROSDIR})
  message(STATUS "MAXCOMPILERDIR and MAXELEROSDIR are set, adding includes!")
  message(STATUS "  MAXCOMPILER --> $ENV{MAXCOMPILERDIR}")
  message(STATUS "  MAXELEROS   --> $ENV{MAXELEROSDIR}")
  include_directories ($ENV{MAXCOMPILERDIR}/include)
  include_directories ($ENV{MAXCOMPILERDIR}/include/slic)
  include_directories ($ENV{MAXELEROSDIR}/include)
  link_directories($ENV{MAXCOMPILERDIR}/lib)
  link_directories($ENV{MAXELEROSDIR}/lib)

  # --- Maxeler targets
  add_custom_target(
    blas_dmvm_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runsim)
  add_custom_target(
    blas_dmvm_fpga_dfe
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build run)

  # --- Tests
  add_test(
    NAME test_blas_dmvm_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runsim)
  add_test(
    NAME test_blas_sparse_dot_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/SparseDotProduct/build runsim)
  add_test(
    NAME test_blas_dltsolve_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/DenseLowerTriangularSolve/build runsim)
  add_test(
    NAME test_blas_jacobi_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/Jacobi/build runsim)
  add_test(
    NAME test_blas_naive_dmmm
    COMMAND make -C ../test/LinearAlgebra/NaiveMatrixMultiply/build runsim)
  add_test(
    NAME test_sorting_bitonic_fpga_sim
    COMMAND make -C ../test/Sorting/BitonicSorting/build runsim)
  add_test(
    NAME test_sorting_parallel_bitonic_sort_fpga_sim
    COMMAND make -C ../test/Sorting/ParallelBitonicSort/build runsim)
  add_test(
    NAME test_reductions_log_addreduce
    COMMAND make -C ../test/Summation/LogAddReduce/build runsim)
  add_test(
    NAME test_reductions_balanced_tree
    COMMAND make -C ../test/Summation/FullSummationBalancedTree/build runsim)
else()
  message(STATUS "MAXCOMPILERDIR not set!")
  add_custom_target(
    blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
  add_test(
    NAME test_blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
endif()

