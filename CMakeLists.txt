cmake_minimum_required (VERSION 3.10.2)
project (pakman)

# CTest
include (CTest)

# Set C++11 standard flag
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Subdirectories
add_subdirectory (samplers)
add_subdirectory (densities)
add_subdirectory (simulators)
add_subdirectory (methods)
add_subdirectory (include)
add_subdirectory (examples)
add_subdirectory (utils)
