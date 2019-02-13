cmake_minimum_required (VERSION 3.10.2)
project (pakman)

# Find MPI
include (FindMPI)

if (NOT MPI_C_FOUND)
    message (FATAL_ERROR "MPI installation with C bindings was not found")
endif (NOT MPI_C_FOUND)

# Set C++11 standard
if (NOT CMAKE_CXX_COMPILE_FEATURES MATCHES "cxx_std_11")
    message (FATAL_ERROR "Found compiler does not support C++11")
endif ()

set (CMAKE_CXX_STANDARD 11)

# Export symbols for backtrace if build type is not given, Debug,
# RelWithDebInfo
if (CMAKE_BUILD_TYPE MATCHES "|Debug|RelWithDebInfo")
    set (CMAKE_ENABLE_EXPORTS TRUE)
endif()

# CTest
include (CTest)

# Subdirectories
add_subdirectory (samplers)
add_subdirectory (densities)
add_subdirectory (methods)
add_subdirectory (include)
add_subdirectory (utils)
add_subdirectory (tests)
add_subdirectory (examples)
