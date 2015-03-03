cmake_minimum_required (VERSION 2.6)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -march=native -funroll-loops -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

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

set (PRJ "SpmvBase")
set (MANAGER "fpgaNaiveManager")

set (DFE_SOURCES ../src/*.maxj ../lib/lib-reductions/include/reductions/*.maxj)

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
  runsim
  COMMAND maxcompilersim -n ${USER}a -c${DFE_MODEL} -d${NUMDEVICES} restart
  COMMAND SLIC_CONF=${SLIC_CONF} LD_PRELOAD=${MAXOS_SIM} bin/${PRJ}_sim ${USER}a0:${USER}a \${FILE} \${NUM_REPEATS}
  DEPENDS ${PRJ}_sim
  )

add_custom_target(
  gdbsim
  COMMAND maxcompilersim -n ${USER}a -c${DFE_MODEL} -d${NUMDEVICES} restart
  COMMAND SLIC_CONF=${SLIC_CONF} LD_PRELOAD=${MAXOS_SIM} gdb --args bin/${PRJ}_sim ${USER}a0:${USER}a \${FILE} \${NUM_REPEATS}
  DEPENDS ${PRJ}_sim
  )

add_custom_command(
  OUTPUT ${PRJ}_sim.o
  DEPENDS src/*.maxj
  COMMAND maxjc -cp $ENV{MAXCOMPILERDIR}/lib/MaxCompiler.jar -1.6 -d . ${DFE_SOURCES}
  C0OMMAND MAXAPPJCP=. MAXSOURCEDIRS='../src' maxJavaRun -v -m 8192 ${MANAGER} DFEModel=${DFE_MODEL} maxFileName=${PRJ} target='DFE_SIM' enableMPCX=true | tee output.log
  COMMAND grep 'Build location:' output.log | sed 's/.*Build location:[[:space:]]*//' > buildlocation
  COMMAND mkdir -p "include" && cp `cat buildlocation`"/results/${PRJ}.h" "include/Maxfiles.h"
  COMMAND sliccompile `cat buildlocation`"/results/${PRJ}.max" ${PRJ}_sim.o
  )

add_custom_command(
  OUTPUT ${PRJ}_dfe.o
  COMMAND maxjc -cp $ENV{MAXCOMPILERDIR}/lib/MaxCompiler.jar -1.6 -d . ${DFE_SOURCES}
  COMMAND MAXAPPJCP=. MAXSOURCEDIRS='../src' maxJavaRun -v -m 8192 ${MANAGER} DFEModel=${DFE_MODEL} maxFileName=${PRJ} target='DFE' enableMPCX=true
  COMMAND mkdir -p "include" && cp "${MAXBUILD_DIR_HW}/results/${PRJ}.h" "include/Maxfiles.h"
  COMMAND sliccompile "${MAXBUILD_DIR_HW}/results/${PRJ}.max" ${PRJ}_sim.o
  DEPENDS src/*.maxj
  )

if (DEFINED ENV{MAXCOMPILERDIR} AND DEFINED ENV{MAXELEROSDIR})
  message(STATUS "MAXCOMPILERDIR and MAXELEROSDIR are set, adding includes!")
  message(STATUS "  MAXCOMPILER --> $ENV{MAXCOMPILERDIR}")
  message(STATUS "  MAXELEROS   --> $ENV{MAXELEROSDIR}")
  include_directories ($ENV{MAXCOMPILERDIR}/include)
  include_directories ($ENV{MAXCOMPILERDIR}/include/slic)
  include_directories ($ENV{MAXELEROSDIR}/include)
  link_directories($ENV{MAXCOMPILERDIR}/lib)
  link_directories($ENV{MAXELEROSDIR}/lib)
else()
  message(STATUS "MAXCOMPILERDIR not set!")
endif()

# --- Include MKL ---

include(FindMKL.cmake)
INCLUDE_DIRECTORIES( ${MKL_INCLUDE_DIR} )

# -- Library includes --
include_directories (include/eval)
include_directories (src)
include_directories (build/include)
include_directories (lib/spmv-commons-cpu)

file(GLOB CppSources src/*.cpp)
file(GLOB LibrarySources lib/spmv-commons-cpu/*.cpp)

add_library(spmv_commons_cpu SHARED ${LibrarySources})

add_executable(${PRJ}_sim src/fpgaNaiveCpuCode.cpp ${PRJ}_sim.o)
target_link_libraries(${PRJ}_sim maxeleros slic m pthread ${MKL} spmv_commons_cpu)

add_executable(${PRJ}_dfe src/main.cpp ${PRJ}_dfe.o)
target_link_libraries(${PRJ}_dfe maxeleros slic m pthread ${MKL} spmv_commons_cpu)
