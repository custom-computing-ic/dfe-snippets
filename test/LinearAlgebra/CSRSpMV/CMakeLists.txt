cmake_minimum_required (VERSION 2.6)

set (PRJ "SpmvBase")
set (MANAGER "fpgaNaiveManager")
set (DFE_SNIPPETS_LIB "${CMAKE_CURRENT_LIST_DIR}/../../../src/")
set (DFE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.maxj)

# include(${PROJECT_SOURCE_DIR}/scripts/MaxelerUtils.cmake)

function(add_matrix_test name )
  set(exepath ${EXECUTABLE_OUTPUT_PATH}/LinearAlgebraCSRSpMV_sim)
  add_test(
    NAME test_LinearAlgebraCSRSpMV_${name}
    COMMAND ./run_single.sh ${CMAKE_CURRENT_LIST_DIR}/test-matrices/test_${name}.mtx ${exepath}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/scripts)
endfunction()


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

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

# -- MKL
set(INSTALL_TO_DIR ${CMAKE_SOURCE_DIR}/build)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/benchmark/lib )
INCLUDE_DIRECTORIES( ${MKL_INCLUDE_DIR} )

# Add MaxCompiler Benchmarks
# DFE Models: MAX3424A or MAIA
set(DFE_MODEL "$ENV{DFE_MODEL}")
if (DFE_MODEL)
  message(STATUS "Using specified DFE Model --> $ENV{DFE_MODEL}")
else (DFE_MODEL)
  set(DFE_MODEL MAIA)
  message(STATUS "Using default DFE Model --> MAIA")
endif (DFE_MODEL)


set (SIMMAXDIR "${PRJ}_${DFE_MODEL}_DFE_SIM/results")
set (HWMAXDIR "${PRJ}_${DFE_MODEL}_DFE/results")
set (SIMMAXFILE "${SIMMAXDIR}/${PRJ}.max")
set (HWMAXFILE "${HWMAXDIR}/${PRJ}.max")

set (NUMDEVICES "1")
set (MAXELEROSDIR_SIM "$ENV{MAXCOMPILERDIR}/lib/maxeleros-sim")
set (MAXOS_SIM "${MAXELEROSDIR_SIM}/lib/libmaxeleros.so")
set (USER "$ENV{USER}")
set (SIMNAME "${USER}a0:${USER}a")
set (SLIC_CONF "use_simulation=${USER}a")

execute_process(
  COMMAND maxGuessBuildDir -n ${PRJ}_${DFE_MODEL}_DFE_SIM
  OUTPUT_VARIABLE MAXBUILD_DIR_SIM
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND maxGuessBuildDir -n ${PRJ}_DFE_SIM
  OUTPUT_VARIABLE MAXBUILD_DIR_HW
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

message(STATUS "Simulation Build DIR --> ${MAXBUILD_DIR_SIM}")
message(STATUS "Hardware   Build DIR --> ${MAXBUILD_DIR_HW}")

add_custom_target(
  gdbsim
  COMMAND maxcompilersim -n ${USER}a -c${DFE_MODEL} -d${NUMDEVICES} restart
  COMMAND SLIC_CONF=${SLIC_CONF} LD_PRELOAD=${MAXOS_SIM} gdb --args ./${PRJ}_sim ${USER}a0:${USER}a \${FILE} \${NUM_REPEATS}
  DEPENDS ${PRJ}_sim
  )

add_custom_command(
  OUTPUT ${PRJ}_dfe.o
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.maxj
  COMMAND maxjc -cp $ENV{MAXCOMPILERDIR}/lib/MaxCompiler.jar:${DFE_SNIPPETS_LIB} -1.6 -d . ${DFE_SOURCES}
  COMMAND MAXAPPJCP=. MAXSOURCEDIRS='${CMAKE_CURRENT_LIST_DIR}/src' maxJavaRun -v -m 8192 ${MANAGER} DFEModel=${DFE_MODEL} maxFileName=${PRJ} target='DFE' enableMPCX=true
  COMMAND grep 'Build location:' output.log | sed 's/.*Build location:[[:space:]]*//' > buildlocation
  COMMAND mkdir -p "include" && cp `cat buildlocation`"/results/${PRJ}.h" "include/Maxfiles.h"
  COMMAND sliccompile `cat buildlocation`"/results/${PRJ}.max" ${PRJ}_sim.o
  )

# --- Include MKL ---

include(${PROJECT_SOURCE_DIR}/scripts/FindMKL.cmake)
include_directories(${MKL_INCLUDE_DIR})

# -- Library includes --
include_directories (include/eval)
include_directories (${CMAKE_CURRENT_LIST_DIR}/src)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories (../../../include/)

file(GLOB CppSources ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
#file(GLOB LibrarySources ../../../include/dfesnippets/sparse/*.cpp)

#add_library(spmv_commons_cpu SHARED ${LibrarySources})


add_executable(${PRJ}_dfe ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp ${PRJ}_dfe.o)
target_link_libraries(${PRJ}_dfe maxeleros slic m pthread ${MKL})

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/cpu/)

# -- Tests

enable_testing()

add_matrix_test("break")
add_matrix_test("cage6")
add_matrix_test("long_row")
add_matrix_test("no_empty_rows")
add_matrix_test("non_multiple")
add_matrix_test("one_row")
add_matrix_test("one_value_one_index_per_row")
add_matrix_test("small")
add_matrix_test("some_empty_rows")
add_matrix_test("tiny")
add_matrix_test("tols90")
add_matrix_test("two_rows")
add_matrix_test("tiny_odd")
