cmake_minimum_required(VERSION 2.6)

project("dfe-snippets")

if (DEFINED ENV{MAXCOMPILERDIR} AND DEFINED ENV{MAXELEROSDIR})
  message(STATUS "MAXCOMPILERDIR and MAXELEROSDIR are set, adding includes!")
  message(STATUS "  MAXCOMPILER --> $ENV{MAXCOMPILERDIR}")
  message(STATUS "  MAXELEROS   --> $ENV{MAXELEROSDIR}")
  include_directories ($ENV{MAXCOMPILERDIR}/include)
  include_directories ($ENV{MAXCOMPILERDIR}/include/slic)
  include_directories ($ENV{MAXELEROSDIR}/include)
  link_directories($ENV{MAXCOMPILERDIR}/lib)
  link_directories($ENV{MAXELEROSDIR}/lib)
  add_custom_target(
    blas_dmvm_fpga
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runsim)
else()
  message(STATUS "MAXCOMPILERDIR not set!")
  add_custom_target(
    blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
endif()

