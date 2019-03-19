cmake_minimum_required (VERSION 3.10.2)
project (pakman)

# Find MPI
include (FindMPI)

if (NOT MPI_C_FOUND)
    message (FATAL_ERROR "MPI installation with C bindings was not found")
endif (NOT MPI_C_FOUND)

# If hosts flags are given, add them to MPIEXEC_PREFLAGS
if (MPIEXEC_HOSTS_FLAGS)
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} ${MPIEXEC_HOSTS_FLAGS}")
    message ("Host flags are: ${MPIEXEC_HOSTS_FLAGS}")
endif ()

# Find Python
include (FindPythonInterp)

if (NOT PYTHONINTERP_FOUND)
    message (WARNING "Python interpreter was not found, "
        "cannot generate plots")
endif ()

# If the MPI installation is OpenMPI, make sure to add `--oversubscribe` flags
# to MPIEXEC_PREFLAGS
execute_process (COMMAND ${MPIEXEC_EXECUTABLE} --version
    OUTPUT_VARIABLE mpiexec_version)

if (mpiexec_version MATCHES "OpenRTE")
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} --oversubscribe")
endif ()

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

# Include CTest
include (CTest)

# Include custom cmake modules
include ("${PROJECT_SOURCE_DIR}/cmake/ConfigureScript.cmake")

# Subdirectories
add_subdirectory (samplers)
add_subdirectory (densities)
add_subdirectory (src)
add_subdirectory (include)
add_subdirectory (utils)
add_subdirectory (tests)
add_subdirectory (examples)
add_subdirectory (scaling)
