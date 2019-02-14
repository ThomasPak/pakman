# Add standard-simulator
add_executable (standard-simulator standard-simulator.c)

#####################
## Test sweep mode ##
#####################
## MPI Master
# Test if output matches expected output
add_sweep_match_test (
    MPI                     # Master type
    Standard                # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

# Test if Pakman throws error when simulator throws error
add_sweep_error_test (
    MPI                     # Master type
    Standard                # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

## Serial Master
# Test if output matches expected output
add_sweep_match_test (
    Serial                  # Master type
    Standard                # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

# Test if Pakman throws error when simulator throws error
add_sweep_error_test (
    Serial                  # Master type
    Standard                # Simulator type
    ""                      # Postfix
    p                       # Parameter name
    "1\\n2\\n3\\n4\\n5"     # Parameter list
    )

#########################
## Test rejection mode ##
#########################
## MPI Master
# Test if output matches expected output
add_rejection_match_test (
    MPI         # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_rejection_error_test (
    MPI         # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

## Serial Master
# Test if output matches expected output
add_rejection_match_test (
    Serial      # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_rejection_error_test (
    Serial      # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

###################
## Test smc mode ##
###################
## MPI Master
# Test if output matches expected output
add_smc_match_test (
    MPI         # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_smc_error_test (
    MPI         # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

## Serial Master
# Test if output matches expected output
add_smc_match_test (
    Serial      # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )

# Test if Pakman throws error when simulator throws error
add_smc_error_test (
    Serial      # Master type
    Standard    # Simulator type
    ""          # Postfix
    10          # Number of parameters
    p           # Parameter name
    1           # Sampled parameter
    )
