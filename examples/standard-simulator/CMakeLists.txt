# Find MPI
include (FindMPI)

# Add standard-simulator
add_executable (standard-simulator standard-simulator.c)

#####################
## Test sweep mode ##
#####################
# Simulator does not return error
set (return_code 0)
set (parameter_name p)
set (parameter_list "1\\n2\\n3\\n4\\n5")
set (input_file standard-simulator-no-error.sweep)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.sweep.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Compute expected output
string (REPLACE "\\n" "\n" expected_output "${parameter_name}\n${parameter_list}")

# Add test
# Because pakman runs simulations in parallel, debugging messages in stderr may
# pop up in the stdout results.  Hence we need to throw away stderr in order
# for CTest to match the expected output string successfully.
add_test (StandardSimulatorSweepNoError
    sh -c "${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} \
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/sweep-mpi \
    ${input_file} 2>/dev/null")
set_property (TEST StandardSimulatorSweepNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file standard-simulator-with-error.sweep)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.sweep.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (StandardSimulatorSweepWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/sweep-mpi
    ${input_file})
set_property (TEST StandardSimulatorSweepWithError
    PROPERTY WILL_FAIL TRUE)

#########################
## Test rejection mode ##
#########################
# Simulator does not return error
set (number_of_parameters 10)
set (return_code 0)
set (parameter_name p)
set (sampled_parameter 1)
set (input_file standard-simulator-no-error.rejection)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.rejection.in"
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
add_test (StandardSimulatorRejectionNoError
    sh -c "${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} \
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/rejection-mpi \
    ${input_file} ${number_of_parameters} 2>/dev/null")
set_property (TEST StandardSimulatorRejectionNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file standard-simulator-with-error.rejection)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.rejection.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (StandardSimulatorRejectionWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/rejection-mpi
    ${input_file} ${number_of_parameters})
set_property (TEST StandardSimulatorRejectionWithError
    PROPERTY WILL_FAIL TRUE)

#####################
## Test SMC mode ##
#####################
# Simulator does not return error
set (number_of_parameters 10)
set (return_code 0)
set (parameter_name p)
set (sampled_parameter 1)
set (input_file standard-simulator-no-error.smc)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.smc.in"
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
add_test (StandardSimulatorSMCNoError
    sh -c "${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} \
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/smc-mpi \
    ${input_file} ${number_of_parameters} 2>/dev/null")
set_property (TEST StandardSimulatorSMCNoError
    PROPERTY PASS_REGULAR_EXPRESSION "${expected_output}")

# Simulator returns error
set (return_code 1)
set (input_file standard-simulator-with-error.smc)

# Generate input file
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/standard-simulator.smc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
    )

# Add test
add_test (StandardSimulatorSMCWithError
    ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
    ${MPIEXEC_PREFLAGS} ${PROJECT_BINARY_DIR}/methods/smc-mpi
    ${input_file} ${number_of_parameters})
set_property (TEST StandardSimulatorSMCWithError
    PROPERTY WILL_FAIL TRUE)
