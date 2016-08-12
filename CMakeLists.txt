cmake_minimum_required(VERSION 2.6)


project("dfe-snippets")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# -- Boost
SET (BOOST_ROOT "/opt/boost/boost_1_57_0")
SET (BOOST_INCLUDEDIR "/opt/boost/boost-1.57.0/include")
SET (BOOST_LIBRARYDIR "/opt/boost/boost-1.57.0/lib")
SET (BOOST_MIN_VERSION "1.57.0")
set (Boost_NO_BOOST_CMAKE ON)
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED)
if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.41) required.")
else()
  message(STATUS "Setting up BOOST")
  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif (NOT Boost_FOUND)

# --- Include MKL
include(${PROJECT_SOURCE_DIR}/scripts/FindMKL.cmake)
include_directories(${MKL_INCLUDE_DIR})

# DFE Models: MAX3424A or MAIA
set(DFE_MODEL "$ENV{DFE_MODEL}")
if (DFE_MODEL)
  message(STATUS "Using specified DFE Model --> $ENV{DFE_MODEL}")
else (DFE_MODEL)
  set(DFE_MODEL MAIA)
  message(STATUS "Using default DFE Model --> MAIA")
endif (DFE_MODEL)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(include/)

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

function (add_fpga_build concept name maxfile manager)
  set(PRJ "${concept}${name}")
  set(DFE_SNIPPETS_LIB "${PROJECT_SOURCE_DIR}/src/")
  set(DFE_SRC_DIR "${PROJECT_SOURCE_DIR}/test/${concept}/${name}/src")
  set(DFE_SOURCES ${DFE_SRC_DIR}/*.maxj)
  set(NUMDEVICES 1)
  set(USER $ENV{USER})
  set (SLIC_CONF "use_simulation=${USER}a")
  add_custom_target(
    runsim_${PRJ}
    COMMAND maxcompilersim -n ${USER}a -c${DFE_MODEL} -d${NUMDEVICES} restart
    COMMAND SLIC_CONF=${SLIC_CONF} LD_PRELOAD=${MAXOS_SIM} ./${PRJ}_sim ${USER}a0:${USER}a \${FILE} \${NUM_REPEATS}
    DEPENDS ${PRJ}_sim
    )

  add_custom_command(
    OUTPUT ${PRJ}_sim.o
    DEPENDS ${DFE_SRC_DIR}/*.maxj
    COMMAND maxjc -cp $ENV{MAXCOMPILERDIR}/lib/MaxCompiler.jar:${DFE_SNIPPETS_LIB} -1.6 -d . ${DFE_SOURCES}
    COMMAND MAXAPPJCP=. MAXSOURCEDIRS='${DFE_SRC_DIR}' maxJavaRun -v -m 8192 ${manager} DFEModel=${DFE_MODEL} maxFileName=${maxfile} target='DFE_SIM' enableMPCX=true | tee output.log
    COMMAND grep 'Build location:' output.log | sed 's/.*Build location:[[:space:]]*//' > buildlocation
    COMMAND mkdir -p "include" && cp `cat buildlocation`"/results/${maxfile}.h" "${CMAKE_CURRENT_BINARY_DIR}/include/"
    COMMAND sliccompile `cat buildlocation`"/results/${maxfile}.max" ${PRJ}_sim.o
    )

  add_executable(${PRJ}_sim test/${concept}/${name}/src/${name}CpuCode.cpp ${PRJ}_sim.o)
  target_link_libraries(${PRJ}_sim maxeleros slic m pthread ${MKL})

  add_custom_command(
    OUTPUT ${PRJ}_dfe.o
    DEPENDS ${DFE_SRC_DIR}/*.maxj
    COMMAND maxjc -cp $ENV{MAXCOMPILERDIR}/lib/MaxCompiler.jar:${DFE_SNIPPETS_LIB} -1.6 -d . ${DFE_SOURCES}
    COMMAND MAXAPPJCP=. MAXSOURCEDIRS='${DFE_SRC_DIR}' maxJavaRun -v -m 8192 ${manager} DFEModel=${DFE_MODEL} maxFileName=${PRJ} target='DFE' enableMPCX=true
    COMMAND grep 'Build location:' output.log | sed 's/.*Build location:[[:space:]]*//' > buildlocation
    COMMAND mkdir -p "include" && cp `cat buildlocation`"/results/${maxfile}.h" "include/"
    COMMAND sliccompile `cat buildlocation`"/results/${maxfile}.max" ${PRJ}_sim.o
    )

  add_executable(${PRJ}_dfe test/${concept}/${name}/src/${name}CpuCode.cpp ${PRJ}_dfe.o)
  target_link_libraries(${PRJ}_dfe maxeleros slic m pthread ${MKL})
endfunction()

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

  add_fpga_build(LinearAlgebra AdjustedCSRSpMV SpmvBase fpgaNaiveManager)
  # add_fpga_build(LinearAlgebra CsrDecoder CsrDecoder CsrDecoderManager)
  # add_fpga_build(LinearAlgebra ParallelCsrDecoder ParallelCsrDecoder ParallelCsrDecoderManager)
  add_fpga_build(Tests DramAccumulator DramAccumulator DramAccumulatorManager)

  # --- Tests
  add_fpga_test(LinearAlgebra DenseMatrixVectorMultiply)
  add_fpga_test(LinearAlgebra SparseDotProduct)
  add_fpga_test(LinearAlgebra DenseLowerTriangularSolve)
  # add_fpga_test(LinearAlgebra Jacobi)
  add_fpga_test(LinearAlgebra NaiveMatrixMultiply)
  # add_fpga_test(LinearAlgebra LUSolver)
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
  add_fpga_test(Tests DramAccumulator)
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
  # add_fpga_test(Infrastructure MaxRingMulticast)
  add_fpga_test(Infrastructure ParallelMovingAverage)
  add_fpga_test(DataStructures CircularBuffer)
  add_fpga_test(DataStructures DFEsmFifo)
  add_fpga_test(DataStructures FetchSubTuple)
  add_fpga_test(DataStructures FifoAsShiftRegister)
  #  add_fpga_test(Tests TerminationCondition)

  add_fpga_test(Benchmarks DramBandwidthBenchmarkDefaultMemCtrl)
  add_fpga_test(Benchmarks DramBandwidthMultiMemChannels)
  add_fpga_test(Benchmarks InfinibandBenchmark)

  # TODO
  #add_test(
    #NAME test_csrspmv_fpga_sim
    #COMMAND make -C ../test/LinearAlgebra/CSRSpMV/build runsim)

  add_test(
    NAME test_blas_dmvm_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runsim)
  add_custom_target(
    blas_lusolver_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/LUSolver/build runsim)
else()
  message(STATUS "MAXCOMPILERDIR not set!")
  add_custom_target(
    blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
  add_custom_target(
    blas_lusolver_cpu
    COMMAND make -C ../test/LinearAlgebra/LUSolver/build runcpu)
  add_test(
    NAME test_blas_dmvm_cpu
    COMMAND make -C ../test/LinearAlgebra/DenseMatrixVectorMultiply/build runcpu)
  add_test(
    NAME test_blas_lusolver_fpga_sim
    COMMAND make -C ../test/LinearAlgebra/LUSolver/build runcpu)
endif()


# add_subdirectory(
#   test/LinearAlgebra/AdjustedCSRSpMV)
