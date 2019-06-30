## Helper functions ##

# Function to make base command
function (get_base_command _command
        master
        controller
        simulator
        test_type)

    # Correct for spaces in project path
    string (REPLACE " " "\\ " PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}")

    # Set executable
    set (executable "${PROJECT_BINARY_DIR}/src/pakman")

    # Initialize empty command
    set (command "")

    # Append command based on master type
    if (master MATCHES "Serial")
        string (APPEND command "${executable} serial ")
    elseif (master MATCHES "MPI")
        string (APPEND command "${MPIEXEC_EXECUTABLE} \
        ${MPIEXEC_NUMPROC_FLAG} \
        ${MPIEXEC_MAX_NUMPROCS} \
        ${MPIEXEC_PREFLAGS} \
        ${executable} mpi ")
    endif ()

    # Append command based on controller type
    if (controller MATCHES "Sweep")
        string (APPEND command "sweep ")
    elseif (controller MATCHES "Rejection")
        string (APPEND command "rejection ")
    elseif (controller MATCHES "SMC")
        string (APPEND command "smc ")
    endif ()

    # Append command based on simulator type
    if (simulator MATCHES "MPI")
        string (APPEND command "--mpi-simulator ")
    endif ()

    # Append command based on force_host_spawn
    if (force_host_spawn)
        string (APPEND command "--force-host-spawn ")
    endif ()

    # Append command with --verbosity off if test type is match
    string (APPEND command "--verbosity=off ")

    # Add population size (additional argument) if rejection or smc
    if (controller MATCHES "Rejection|SMC")
        string (APPEND command ${ARGN})
    endif ()

    # Separate arguments
    separate_arguments (command UNIX_COMMAND "${command}")

    # Set _command
    set (${_command} ${command} PARENT_SCOPE)
endfunction ()

## Add test functions ##

# Function to add match test
function (add_match_test
        name
        _command
        expected_output)
    add_test (NAME ${name} COMMAND ${${_command}})
    set_property (TEST ${name}
        PROPERTY PASS_REGULAR_EXPRESSION ${expected_output})
endfunction ()

# Function to add error test
function (add_error_test
        name
        _command)
    add_test (NAME ${name} COMMAND ${${_command}})
    set_property (TEST ${name}
        PROPERTY PASS_REGULAR_EXPRESSION "Task finished with error!")
endfunction ()

## Sweep functions ##

# Function to make sweep options
function (get_sweep_options _options
        simulator
        postfix
        return_code
        parameter_names
        parameter_list)

    # Correct for spaces in project path
    string (REPLACE " " "\\\\ " PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}")

    # Initialize empty options
    set (options "")

    # Append options with parameter_names
    string (APPEND options "--parameter-names=\"${parameter_names}\" ")

    # Append options with generator
    string (REPLACE "\\n" "\\\\n" parameter_list "${parameter_list}")
    string (APPEND options "--generator=\"printf '${parameter_list}\\\\n'\" ")

    # Append options with simulator
    string (APPEND options "--simulator=\"")

    if (simulator MATCHES "Standard")
        string (APPEND options
            "${PROJECT_BINARY_DIR}/tests/standard-simulator/standard-simulator ")
    elseif (simulator MATCHES "MPI")
        if (postfix MATCHES "Cpp")
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator-cpp ")
        else ()
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator ")
        endif ()
    endif ()

    string (APPEND options "'' ${return_code}\"")

    # Separate arguments
    separate_arguments (options UNIX_COMMAND ${options})

    # Set _command
    set (${_options} ${options} PARENT_SCOPE)
endfunction ()

# Function to get sweep expected output
function (get_sweep_expected_output _expected_output
        parameter_names parameter_list)
    string (REPLACE "\\n" "\n" expected_output
        "${parameter_names}\n${parameter_list}")
    set (${_expected_output} ${expected_output} PARENT_SCOPE)
endfunction ()

# Function for adding sweep match test
function (add_sweep_match_test
        master
        simulator
        postfix
        parameter_names
        parameter_list)

    # Set name
    set (name "${master}MasterSweep${simulator}SimulatorMatch${postfix}")

    # Compute expected output
    get_sweep_expected_output (expected_output ${parameter_names} ${parameter_list})

    # Get base command
    get_base_command (command ${master} Sweep ${simulator} Match)

    # Get sweep options
    get_sweep_options (options ${simulator} "${postfix}" 0 ${parameter_names}
        ${parameter_list})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding sweep error test
function (add_sweep_error_test
        master
        simulator
        postfix
        parameter_names
        parameter_list)

    # Set name
    set (name "${master}MasterSweep${simulator}SimulatorError${postfix}")

    # Get base command
    get_base_command (command ${master} Sweep ${simulator} Error)

    # Get sweep options
    get_sweep_options (options ${simulator} "${postfix}" 1 ${parameter_names}
        ${parameter_list})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_error_test (${name} command)
endfunction ()

## Rejection functions ##

# Function to make rejection options
function (get_rejection_options _options
        simulator
        postfix
        return_code
        parameter_names
        sampled_parameter
        number_of_parameters)

    # Correct for spaces in project path
    string (REPLACE " " "\\\\ " PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}")

    # Initialize empty options
    set (options "")

    # Append options with parameter_names
    string (APPEND options "--parameter-names=\"${parameter_names}\" ")

    # Append options with prior sampler
    string (APPEND options "--prior-sampler=\"echo ${sampled_parameter}\" ")

    # Append options with number of parameters
    string (APPEND options "--number-accept=${number_of_parameters} ")

    # Append options with epsilon
    string (APPEND options "--epsilon=0 ")

    # Append options with simulator
    string (APPEND options "--simulator=\"")

    if (simulator MATCHES "Standard")
        string (APPEND options
            "${PROJECT_BINARY_DIR}/tests/standard-simulator/standard-simulator ")
    elseif (simulator MATCHES "MPI")
        if (postfix MATCHES "Cpp")
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator-cpp ")
        else ()
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator ")
        endif ()
    endif ()

    string (APPEND options "1 ${return_code}\"")

    # Separate arguments
    separate_arguments (options UNIX_COMMAND ${options})

    # Set _command
    set (${_options} ${options} PARENT_SCOPE)
endfunction ()

# Function to get rejection expected output
function (get_rejection_expected_output _expected_output
        number_of_parameters
        parameter_names
        sampled_parameter)

    set (expected_output "${parameter_names}")
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
        parameter_names
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterRejection${simulator}SimulatorMatch${postfix}")

    # Compute expected output
    get_rejection_expected_output (expected_output
        ${number_of_parameters}
        ${parameter_names}
        ${sampled_parameter})

    # Get base command
    get_base_command (command ${master} Rejection ${simulator} Match)

    # Get rejection options
    get_rejection_options (options ${simulator} "${postfix}" 0 ${parameter_names}
        ${sampled_parameter} ${number_of_parameters})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding rejection error test
function (add_rejection_error_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_names
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterRejection${simulator}SimulatorError${postfix}")

    # Get base command
    get_base_command (command ${master} Rejection ${simulator} Error)

    # Get rejection options
    get_rejection_options (options ${simulator} "${postfix}" 1 ${parameter_names}
        ${sampled_parameter} ${number_of_parameters})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_error_test (${name} command)
endfunction ()

## SMC functions ##

# Function to make smc options
function (get_smc_options _options
        simulator
        postfix
        return_code
        parameter_names
        sampled_parameter
        number_of_parameters)

    # Correct for spaces in project path
    string (REPLACE " " "\\\\ " PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}")

    # Initialize empty options
    set (options "")

    # Append options with parameter_names
    string (APPEND options "--parameter-names=\"${parameter_names}\" ")

    # Append options with prior sampler
    string (APPEND options "--prior-sampler=\"echo ${sampled_parameter}\" ")

    # Append options with perturber
    string (APPEND options "--perturber=\"bash -c 'cat > /dev/null && echo 1'\" ")

    # Append options with prior pdf
    string (APPEND options "--prior-pdf=\"bash -c 'cat > /dev/null && echo 1'\" ")

    # Append options with perturbation pdf
    string (APPEND options "--perturbation-pdf=\"bash -c 'read t && read new_p && cat'\" ")

    # Append options with number of parameters
    string (APPEND options "--population-size=${number_of_parameters} ")

    # Append options with epsilons
    string (APPEND options "--epsilons=2,1,0 ")

    # Append options with simulator
    string (APPEND options "--simulator=\"")

    if (simulator MATCHES "Standard")
        string (APPEND options
            "${PROJECT_BINARY_DIR}/tests/standard-simulator/standard-simulator ")
    elseif (simulator MATCHES "MPI")
        if (postfix MATCHES "Cpp")
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator-cpp ")
        else ()
            string (APPEND options
                "${PROJECT_BINARY_DIR}/tests/mpi-simulator/mpi-simulator ")
        endif ()
    endif ()

    string (APPEND options "1 ${return_code}\"")

    # Separate arguments
    separate_arguments (options UNIX_COMMAND ${options})

    # Set _command
    set (${_options} ${options} PARENT_SCOPE)
endfunction ()

# Function to get smc expected output
function (get_smc_expected_output _expected_output
        number_of_parameters
        parameter_names
        sampled_parameter)

    set (expected_output "${parameter_names}")
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
        parameter_names
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterSMC${simulator}SimulatorMatch${postfix}")

    # Compute expected output
    get_smc_expected_output (expected_output
        ${number_of_parameters}
        ${parameter_names}
        ${sampled_parameter})

    # Get base command
    get_base_command (command ${master} SMC ${simulator} Match)

    # Get smc options
    get_smc_options (options ${simulator} "${postfix}" 0 ${parameter_names}
        ${sampled_parameter} ${number_of_parameters})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_match_test (${name} command ${expected_output})
endfunction ()

# Function for adding smc error test
function (add_smc_error_test
        master
        simulator
        postfix
        number_of_parameters
        parameter_names
        sampled_parameter)

    # Set name and input file
    set (name "${master}MasterSMC${simulator}SimulatorError${postfix}")


    # Get base command
    get_base_command (command ${master} SMC ${simulator} Match)

    # Get smc options
    get_smc_options (options ${simulator} "${postfix}" 1 ${parameter_names}
        ${sampled_parameter} ${number_of_parameters})

    # Append options to base command
    list (APPEND command ${options})

    # Add test
    add_error_test (${name} command)
endfunction ()
