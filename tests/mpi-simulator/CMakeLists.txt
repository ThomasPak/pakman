# If the MPI installation is OpenMPI, make sure to add `--force-host-spawn` flags
# to MPIEXEC_POSTFLAGS
if (mpiexec_version MATCHES "OpenRTE")
    list (APPEND MPIEXEC_POSTFLAGS "--force-host-spawn")
endif ()

include_directories (${MPI_C_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/include")
add_compile_options (${MPI_C_COMPILE_OPTIONS})
link_libraries (${MPI_CXX_LIBRARIES})

# Add mpi-simulator and mpi-simulator-cpp
add_executable (mpi-simulator mpi-simulator.c)
add_executable (mpi-simulator-cpp mpi-simulator-cpp.cc)

# Test run-mpi-simulator
add_test (RunMPISimulatorMatch
    bash -c
    "printf 'epsilon\\\\nparameter\\\\n' > input_file.txt && \
    ${MPIEXEC_EXECUTABLE} \
    ${MPIEXEC_NUMPROC_FLAG} 1 \
    ${MPIEXEC_PREFLAGS} \
    '${PROJECT_BINARY_DIR}/utils/run-mpi-simulator' \
    input_file.txt \
    '${CMAKE_CURRENT_BINARY_DIR}/mpi-simulator' somestring 0")

set_property (TEST RunMPISimulatorMatch
    PROPERTY PASS_REGULAR_EXPRESSION "somestring")

add_test (RunMPISimulatorError
    bash -c
    "printf 'epsilon\\\\nparameter\\\\n' > input_file.txt && \
    ${MPIEXEC_EXECUTABLE} \
    ${MPIEXEC_NUMPROC_FLAG} 1 \
    ${MPIEXEC_PREFLAGS} \
    '${PROJECT_BINARY_DIR}/utils/run-mpi-simulator' \
    input_file.txt \
    '${CMAKE_CURRENT_BINARY_DIR}/mpi-simulator' somestring 1")

set_property (TEST RunMPISimulatorError
    PROPERTY PASS_REGULAR_EXPRESSION "somestring")

add_test (RunMPISimulatorMatchCpp
    bash -c
    "printf 'epsilon\\\\nparameter\\\\n' > input_file.txt && \
    ${MPIEXEC_EXECUTABLE} \
    ${MPIEXEC_NUMPROC_FLAG} 1 \
    ${MPIEXEC_PREFLAGS} \
    '${PROJECT_BINARY_DIR}/utils/run-mpi-simulator' \
    input_file.txt \
    '${CMAKE_CURRENT_BINARY_DIR}/mpi-simulator-cpp' somestring 0")

set_property (TEST RunMPISimulatorMatchCpp
    PROPERTY PASS_REGULAR_EXPRESSION "somestring")

add_test (RunMPISimulatorErrorCpp
    bash -c
    "printf 'epsilon\\\\nparameter\\\\n' > input_file.txt && \
    ${MPIEXEC_EXECUTABLE} \
    ${MPIEXEC_NUMPROC_FLAG} 1 \
    ${MPIEXEC_PREFLAGS} \
    '${PROJECT_BINARY_DIR}/utils/run-mpi-simulator' \
    input_file.txt \
    '${CMAKE_CURRENT_BINARY_DIR}/mpi-simulator-cpp' somestring 1")

set_property (TEST RunMPISimulatorErrorCpp
    PROPERTY PASS_REGULAR_EXPRESSION "somestring")

#####################
## Test sweep mode ##
#####################
## MPI simulator with C header
# Test if output matches expected output
add_sweep_match_test (
    MPI                     # Master type
    MPI                     # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

# Test if Pakman throws error when simulator throws error
add_sweep_error_test (
    MPI                     # Master type
    MPI                     # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

## MPI simulator with C++ header
# Test if output matches expected output
add_sweep_match_test (
    MPI                     # Master type
    MPI                     # Simulator type
    "Cpp"                   # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

# Test if Pakman throws error when simulator throws error
add_sweep_error_test (
    MPI                     # Master type
    MPI                     # Simulator type
    "Cpp"                   # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

#########################
## Test rejection mode ##
#########################
## MPI simulator with C header
# Test if output matches expected output
add_rejection_match_test (
    MPI             # Master type
    MPI             # Simulator type
    ""              # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_rejection_error_test (
    MPI             # Master type
    MPI             # Simulator type
    ""              # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

## MPI simulator with C++ header
# Test if output matches expected output
add_rejection_match_test (
    MPI             # Master type
    MPI             # Simulator type
    "Cpp"           # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_rejection_error_test (
    MPI             # Master type
    MPI             # Simulator type
    "Cpp"           # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

###################
## Test smc mode ##
###################
## MPI simulator with C header
# Test if output matches expected output
add_smc_match_test (
    MPI             # Master type
    MPI             # Simulator type
    ""              # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_smc_error_test (
    MPI             # Master type
    MPI             # Simulator type
    ""              # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

## MPI simulator with C++ header
# Test if output matches expected output
add_smc_match_test (
    MPI             # Master type
    MPI             # Simulator type
    "Cpp"           # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_smc_error_test (
    MPI             # Master type
    MPI             # Simulator type
    "Cpp"           # Postfix
    10              # Number of parameters
    p               # Parameter name
    1               # Sampled parameter
    )
