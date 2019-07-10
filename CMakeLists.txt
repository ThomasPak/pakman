cmake_minimum_required (VERSION 3.10.2)
project (pakman)

# Find MPI
include (FindMPI)

if (NOT MPI_C_FOUND)
    message (FATAL_ERROR "MPI installation with C bindings was not found")
endif (NOT MPI_C_FOUND)

# If hosts flags are given, add them to MPIEXEC_PREFLAGS
set (MPIEXEC_HOSTS_FLAGS "" CACHE STRING "Flags for specifying hosts to mpiexec")
if (NOT ${MPIEXEC_HOSTS_FLAGS} STREQUAL "")
    set (MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS} ${MPIEXEC_HOSTS_FLAGS}")
    message ("Host flags are: ${MPIEXEC_HOSTS_FLAGS}")
endif ()

# When testing in Travis CI, mpiexec will use only on one process by default.
# Since this would preclude Travis CI from testing Pakman with multiple
# processes, we hardcode 2 processes in the Travis CI environment
if ($ENV{TRAVIS})
    set (MPIEXEC_MAX_NUMPROCS 2)
endif ()

# Find Python 3
set (Python_ADDITIONAL_VERSIONS "3")
include (FindPythonInterp)

if (NOT PYTHONINTERP_FOUND)
    message (WARNING "Python interpreter was not found, "
        "cannot generate plots")
endif ()

if (NOT PYTHON_VERSION_MAJOR EQUAL "3")
    message (WARNING "Python version found was ${PYTHON_VERSION_MAJOR}, "
        "but scripts were written for Python 3. Some scripts may fail.")
endif ()

if (PYTHONINTERP_FOUND)
    include ("cmake/FindPythonModule.cmake")

    find_python_module ("numpy" NUMPY_FOUND)
    if (NOT NUMPY_FOUND)
        message (WARNING "Python module 'numpy' was not found, "
            "this may affect Python scripts")
    endif ()

    find_python_module ("matplotlib" MATPLOTLIB_FOUND)
    if (NOT MATPLOTLIB_FOUND)
        message (WARNING "Python module 'matplotlib' was not found, "
            "this may affect Python scripts")

        # Dummy keyword to avoid SyntaxError
        set (NORMED_KEYWORD "normed")
    else ()
        # Set keyword for normed in matplotlib.pyplot.hist based on matplotlib version
        execute_process (COMMAND ${PYTHON_EXECUTABLE} -c
                "import matplotlib; print(matplotlib.__version__)"
                OUTPUT_VARIABLE MATPLOTLIB_VERSION)

        string (REPLACE "." ";" MATPLOTLIB_VERSION ${MATPLOTLIB_VERSION})

        list (GET MATPLOTLIB_VERSION 0 MATPLOTLIB_VERSION_MAJOR)
        list (GET MATPLOTLIB_VERSION 1 MATPLOTLIB_VERSION_MINOR)

        if (MATPLOTLIB_VERSION_MAJOR GREATER "2")
            set (NORMED_KEYWORD "density")
        else ()
            if (MATPLOTLIB_VERSION_MAJOR LESS "2")
                set (NORMED_KEYWORD "normed")
            else ()
                if (MATPLOTLIB_VERSION_MINOR LESS "1")
                    set (NORMED_KEYWORD "normed")
                else ()
                    set (NORMED_KEYWORD "density")
                endif ()
            endif ()
        endif ()
    endif ()
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

# Set CHASTE_DIR and CHASTE_BUILD_DIR as CACHE variables
set (CHASTE_DIR "$ENV{CHASTE_DIR}" CACHE PATH "Path to Chaste source directory")
set (CHASTE_BUILD_DIR "$ENV{CHASTE_BUILD_DIR}" CACHE PATH "Path to Chaste build directory")
set (CHASTE_BUILD_OPTIONS "" CACHE STRING "Options to pass to Chaste build command")

# Include CTest
include (CTest)

# Include custom cmake modules
include ("${PROJECT_SOURCE_DIR}/cmake/ConfigureScript.cmake")

# Subdirectories
add_subdirectory (src)
add_subdirectory (include)
add_subdirectory (tests)
add_subdirectory (examples)
add_subdirectory (scaling)
add_subdirectory (utils)
