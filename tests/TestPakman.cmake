## Helper functions ##

# Function to get current directory
function (get_current_directory _current_dir)
    get_filename_component (
        temp
        ${CMAKE_CURRENT_SOURCE_DIR} NAME
        )
    set (${_current_dir} ${temp} PARENT_SCOPE)
endfunction ()

# Function to get infile for a given controller (sweep, rejection, smc, ...)
function (get_input_infile _infile controller postfix)

    get_current_directory (current_directory)

    string (TOLOWER "${controller}" controller)
    string (TOLOWER "${postfix}" postfix)

    if (postfix)
        string (PREPEND postfix "-")
    endif ()

    set (${_infile} "${current_directory}${postfix}.${controller}.in" PARENT_SCOPE)
endfunction ()

# Function to make test command
function (get_test_command _command
        master
        controller
        simulator
        test_type
        input_file)

    # Set executable based on controller
    # TODO: Remove when pakman executables are unified
    if (controller MATCHES "Sweep")
        set (executable "${PROJECT_BINARY_DIR}/methods/sweep")
    elseif (controller MATCHES "Rejection")
        set (executable "${PROJECT_BINARY_DIR}/methods/rejection")
    elseif (controller MATCHES "SMC")
        set (executable "${PROJECT_BINARY_DIR}/methods/smc")
    endif ()


    # Initialize empty command
    set (command "")

    # If it is type match, discard stderr
    if (test_type MATCHES "Match")
        string (APPEND command "${PROJECT_BINARY_DIR}/utils/discard_stderr.sh ")
    endif ()

    # Append command based on master type
    if (master MATCHES "Serial")
        string (APPEND command "${executable} ")
    elseif (master MATCHES "MPI")
        string (APPEND command "${MPIEXEC_EXECUTABLE} \
        ${MPIEXEC_NUMPROC_FLAG} \
        ${MPIEXEC_MAX_NUMPROCS} \
        ${MPIEXEC_PREFLAGS} \
        ${executable}-mpi \
        ${MPIEXEC_POSTFLAGS} ")
    endif ()

    # Append command based on simulator type
    if (simulator MATCHES "PersistentMPI")
        string (APPEND command "--mpi-simulation --persistent ")
    elseif (simulator MATCHES "MPI")
        string (APPEND command "--mpi-simulation ")
    endif ()

    # Add input file
    string (APPEND command "${input_file} ")

    # Add population size (additional argument) if rejection or smc
    if (controller MATCHES "Rejection|SMC")
        string (APPEND command ${ARGN})
    endif ()

    # Separate arguments
    separate_arguments (command)

    # Set _command
    set (${_command} ${command} PARENT_SCOPE)
endfunction ()

## Add test functions ##

# Function to add match test
function (add_match_test
        name
        _command
        expected_output)
    add_test (${name} ${${_command}})
    set_property (TEST ${name}
        PROPERTY PASS_REGULAR_EXPRESSION ${expected_output})
endfunction ()

# Function to add error test
function (add_error_test
        name
        _command)
    add_test (${name} ${${_command}})
    set_property (TEST ${name}
        PROPERTY WILL_FAIL TRUE)
endfunction ()

## Sweep functions ##

# Function to configure sweep infile
function (configure_sweep_infile
        infile
        input_file
        return_code
        parameter_name
        parameter_list)
    configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/${infile}"
        "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
        )
endfunction ()

# Function to get sweep expected output
function (get_sweep_expected_output _expected_output
        parameter_name parameter_list)
    string (REPLACE "\\n" "\n" expected_output
        "${parameter_name}\n${parameter_list}")
    set (${_expected_output} ${expected_output} PARENT_SCOPE)
endfunction ()

# Function for adding sweep match test
function (add_sweep_match_test
        master
        simulator
        postfix
        parameter_name
        parameter_list)

    # Set name and input file
    set (name "${master}MasterSweep${simulator}SimulatorMatch${postfix}")
    set (input_file "${name}.sweep")

    # Get input infile
    get_input_infile (infile sweep "${postfix}")

    # Generate input file
    set (return_code 0)
    configure_sweep_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${parameter_list}
        )

    # Compute expected output
    get_sweep_expected_output (expected_output ${parameter_name} ${parameter_list})

    # Get test command
    get_test_command (command ${master} Sweep ${simulator} Match ${input_file})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding sweep error test
function (add_sweep_error_test
        master
        simulator
        postfix
        parameter_name
        parameter_list)

    # Set name and input file
    set (name "${master}MasterSweep${simulator}SimulatorError${postfix}")
    set (input_file "${name}.sweep")

    # Get input infile
    get_input_infile (infile sweep "${postfix}")

    # Generate input file
    set (return_code 1)
    configure_sweep_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${parameter_list}
        )

    # Get test command
    get_test_command (command ${master} Sweep ${simulator} Error ${input_file})

    # Add test
    add_error_test (${name} command)
endfunction ()

## Rejection functions ##

# Function to configure rejection infile
function (configure_rejection_infile
        infile
        input_file
        return_code
        parameter_name
        sampled_parameter)
    configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/${infile}"
        "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
        )
endfunction ()

# Function to get rejection expected output
function (get_rejection_expected_output _expected_output
        number_of_parameters
        parameter_name
        sampled_parameter)

    set (expected_output "${parameter_name}")
    set (i 0)
    while (i LESS number_of_parameters)
        string (APPEND expected_output "\n${sampled_parameter}")
        math (EXPR i "${i} + 1")
    endwhile ()

    set (${_expected_output} ${expected_output} PARENT_SCOPE)
endfunction ()

# Function for adding rejection match test
function (add_rejection_match_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_name
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterRejection${simulator}SimulatorMatch${postfix}")
    set (input_file "${name}.rejection")

    # Get input infile
    get_input_infile (infile rejection "${postfix}")

    # Generate input file
    set (return_code 0)
    configure_rejection_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${sampled_parameter}
        )

    # Compute expected output
    get_rejection_expected_output (expected_output
        ${number_of_parameters}
        ${parameter_name}
        ${sampled_parameter})

    # Get test command
    get_test_command (command ${master} Rejection ${simulator} Match ${input_file}
        ${number_of_parameters})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding rejection error test
function (add_rejection_error_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_name
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterRejection${simulator}SimulatorError${postfix}")
    set (input_file "${name}.rejection")

    # Get input infile
    get_input_infile (infile rejection "${postfix}")

    # Generate input file
    set (return_code 1)
    configure_rejection_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${sampled_parameter}
        )

    # Get test command
    get_test_command (command ${master} Rejection ${simulator} Error ${input_file}
        ${number_of_parameters})

    # Add test
    add_error_test (${name} command)
endfunction ()

## SMC functions ##

# Function to configure smc infile
function (configure_smc_infile
        infile
        input_file
        return_code
        parameter_name
        sampled_parameter)
    configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/${infile}"
        "${CMAKE_CURRENT_BINARY_DIR}/${input_file}"
        )
endfunction ()

# Function to get smc expected output
function (get_smc_expected_output _expected_output
        number_of_parameters
        parameter_name
        sampled_parameter)

    set (expected_output "${parameter_name}")
    set (i 0)
    while (i LESS number_of_parameters)
        string (APPEND expected_output "\n${sampled_parameter}")
        math (EXPR i "${i} + 1")
    endwhile ()

    set (${_expected_output} ${expected_output} PARENT_SCOPE)
endfunction ()

# Function for adding smc match test
function (add_smc_match_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_name
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterSMC${simulator}SimulatorMatch${postfix}")
    set (input_file "${name}.smc")

    # Get input infile
    get_input_infile (infile smc "${postfix}")

    # Generate input file
    set (return_code 0)
    configure_smc_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${sampled_parameter}
        )

    # Compute expected output
    get_smc_expected_output (expected_output
        ${number_of_parameters}
        ${parameter_name}
        ${sampled_parameter})

    # Get test command
    get_test_command (command ${master} SMC ${simulator} Match ${input_file}
        ${number_of_parameters})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding smc error test
function (add_smc_error_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_name
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterSMC${simulator}SimulatorError${postfix}")
    set (input_file "${name}.smc")

    # Get input infile
    get_input_infile (infile smc "${postfix}")

    # Generate input file
    set (return_code 1)
    configure_smc_infile (
        ${infile}
        ${input_file}
        ${return_code}
        ${parameter_name}
        ${sampled_parameter}
        )

    # Get test command
    get_test_command (command ${master} SMC ${simulator} Error ${input_file}
        ${number_of_parameters})

    # Add test
    add_error_test (${name} command)
endfunction ()
