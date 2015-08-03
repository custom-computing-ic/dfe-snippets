cmake_minimum_required(VERSION 2.6)

project("dfe-snippets")

enable_testing()

# TODO: clean up builds after tests!
function (add_fpga_test arg1 arg2)
  add_test(
    NAME test_${arg1}_${arg2}_fpga_sim
    COMMAND make -C ../test/${arg1}/${arg2}/build runsim)
endfunction(add_fpga_test)

function (add_fpga_test3 arg1 arg2 arg3)
  add_test(
    NAME test_${arg1}_${arg2}_${arg3}_fpga_sim
    COMMAND make -C ../test/${arg1}/${arg2}/${arg3}/build runsim)
endfunction(add_fpga_test3)

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
  add_fpga_test(LinearAlgebra DenseMatrixVectorMultiply)
  add_fpga_test(LinearAlgebra SparseDotProduct)
  add_fpga_test(LinearAlgebra DenseLowerTriangularSolve)
  add_fpga_test(LinearAlgebra Jacobi)
  add_fpga_test(LinearAlgebra NaiveMatrixMultiply)
  add_fpga_test(Sorting BitonicSorting)
  add_fpga_test(Sorting ParallelBitonicSort)
  add_fpga_test(Summation FullSummationBalancedTree)
  add_fpga_test(Summation FullSummationHardcodedLoopLength)
  add_fpga_test(Summation FullSummationLogarithmicCost)
  add_fpga_test(Summation LogAddReduce)
  add_fpga_test(Summation PartialSummationHardcodedLoopLength)
  add_fpga_test(Summation ReduceStaggeredVectors)
  add_fpga_test(Tests Accumulator)
  add_fpga_test(Tests BRAMCustomBitwidthROM)
  add_fpga_test(Tests BRAMPersistence)
  add_fpga_test(Tests ConnectedKernels)
#  this project is designed not to compile:
#  add_fpga_test(Tests ManagerPropertyPassThrough)
  add_fpga_test(Tests NestedClass)
  add_fpga_test(LanguageFeatures PackUnpack)
  add_fpga_test(LanguageFeatures NestedCounterVariableBoundary)
  add_fpga_test(Infrastructure BRAMReadWrite)
  add_fpga_test(Infrastructure InputDependentCounterIterateUntil)
  #  FIXME:
  #  add_fpga_test(Infrastructure InputDependentCounterTimesToRepeat)
  add_fpga_test(Infrastructure LatencyAnnotation)
  add_fpga_test(Infrastructure LinearMemoryControl)
  add_fpga_test3(Infrastructure LinkingThirdPartyLibrary external_cpp)
  add_fpga_test3(Infrastructure LinkingThirdPartyLibrary external_c)
  add_fpga_test(Infrastructure MaxRingMulticast)
  add_fpga_test(Infrastructure ParallelMovingAverage)
  add_fpga_test(DataStructures CircularBuffer)
  add_fpga_test(DataStructures DFEsmFifo)
  add_fpga_test(DataStructures FetchSubTuple)
  add_fpga_test(DataStructures FifoAsShiftRegister)
  #  add_fpga_test(Tests TerminationCondition)

  # TODO
  #add_test(
    #NAME test_csrspmv_fpga_sim
    #COMMAND make -C ../test/LinearAlgebra/CSRSpMV/build runsim)

else()
  message(STATUS "MAXCOMPILERDIR not set!")
  add_custom_target(
    blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
  add_test(
    NAME test_blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
endif()

