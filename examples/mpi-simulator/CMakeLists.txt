# If the MPI installation is OpenMPI, make sure to add `--force-host-spawn` flags
# to MPIEXEC_POSTFLAGS
if (mpiexec_version MATCHES "OpenRTE")
    list (APPEND MPIEXEC_POSTFLAGS "--force-host-spawn")
endif ()

include_directories (${MPI_C_INCLUDE_DIRS} "${PROJECT_BINARY_DIR}/include")
add_compile_options (${MPI_C_COMPILE_OPTIONS})
link_libraries (${MPI_CXX_LIBRARIES})

# Add mpi-simulator
include_directories ("${PROJECT_BINARY_DIR}/include")
add_executable (mpi-simulator mpi-simulator.c)

#####################
## Test sweep mode ##
#####################
# Simulator does not return error
set (return_code 0)
set (parameter_name p)
set (parameter_list "1\\n2\\n3\\n4\\n5")
set (input_file mpi-simulator-no-error.sweep)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.sweep.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Compute expected output
string (REPLACE "\\n" "\n" expected_output "${parameter_name}\n${parameter_list}")

# Add test
# Because pakman runs simulations in parallel, debugging messages in stderr may
# pop up in the stdout results.  Hence we need to throw away stderr in order
# for CTest to match the expected output string successfully.
add_test (MPISimulatorSweepNoError
    ${PROJECT_BINARY_DIR}/utils/discard_stderr.sh
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/sweep-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} --mpi-simulation)
set_property (TEST MPISimulatorSweepNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file mpi-simulator-with-error.sweep)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.sweep.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (MPISimulatorSweepWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/sweep-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} --mpi-simulation)
set_property (TEST MPISimulatorSweepWithError
    PROPERTY WILL_FAIL TRUE)

#########################
## Test rejection mode ##
#########################
# Simulator does not return error
set (number_of_parameters 10)
set (return_code 0)
set (parameter_name p)
set (sampled_parameter 1)
set (input_file mpi-simulator-no-error.rejection)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.rejection.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Compute expected output
set (expected_output "${parameter_name}")
set (i 0)
while (i LESS number_of_parameters)
    string (APPEND expected_output "\n${sampled_parameter}")
    math (EXPR i "${i} + 1")
endwhile ()

# Add test
add_test (MPISimulatorRejectionNoError
    ${PROJECT_BINARY_DIR}/utils/discard_stderr.sh
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/rejection-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} ${number_of_parameters} --mpi-simulation)
set_property (TEST MPISimulatorRejectionNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file mpi-simulator-with-error.rejection)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.rejection.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (MPISimulatorRejectionWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/rejection-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} ${number_of_parameters} --mpi-simulation)
set_property (TEST MPISimulatorRejectionWithError
    PROPERTY WILL_FAIL TRUE)

#####################
## Test sweep mode ##
#####################
# Simulator does not return error
set (number_of_parameters 10)
set (return_code 0)
set (parameter_name p)
set (sampled_parameter 1)
set (input_file mpi-simulator-no-error.smc)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.smc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Compute expected output
set (expected_output "${parameter_name}")
set (i 0)
while (i LESS number_of_parameters)
    string (APPEND expected_output "\n${sampled_parameter}")
    math (EXPR i "${i} + 1")
endwhile ()

# Add test
add_test (MPISimulatorSMCNoError
    ${PROJECT_BINARY_DIR}/utils/discard_stderr.sh
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/smc-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} ${number_of_parameters} --mpi-simulation)
set_property (TEST MPISimulatorSMCNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file mpi-simulator-with-error.smc)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/mpi-simulator.smc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (MPISimulatorSMCWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/smc-mpi ${MPIEXEC_POSTFLAGS}
    ${input_file} ${number_of_parameters} --mpi-simulation)
set_property (TEST MPISimulatorSMCWithError
    PROPERTY WILL_FAIL TRUE)
