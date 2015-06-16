cmake_minimum_required (VERSION 2.6)

set (PRJ "SpmvCommons")

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
INCLUDE_DIRECTORIES( ${MKL_INCLUDE_DIR} )

# --- Include MKL ---

include(FindMKL.cmake)
INCLUDE_DIRECTORIES( ${MKL_INCLUDE_DIR} )

include_directories(.)

# -- Library includes --
file(GLOB LibrarySources utils.cpp common.cpp mmio.cpp)

# -- Tests
add_library(commons ${LibrarySources})

enable_testing()
add_executable(test_partition test_partition.cpp)
target_link_libraries(test_partition commons ${MKL})


add_test(partition test_partition)
