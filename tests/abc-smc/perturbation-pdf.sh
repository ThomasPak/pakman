#!/bin/bash
set -euo pipefail

# Process arguments
if [ $# -ne 1 ]
then
    echo "Usage: $0 EPSILONS" 1>&2
    echo "Checks parameters and prints 1 if OK" 1>&2
    echo "EPSILONS is comma-separated list of epsilon values" 1>&2
    echo "Throws an error if (parameter + epsilon[t - 1]) is odd" 1>&2
    exit 1
fi

epsilon_list="$1"

# Read t
read t

# Sanity check: t cannot be 0
if [ "$t" -eq "0" ]
then
    echo "t should not be 0" 1>&2
    exit 1
fi

# Get previous epsilon (cut uses one-based indexing for fields)
previous_epsilon=$(echo $epsilon_list | cut -d, -f$t)

# Read perturbed parameter
read perturbed_prmtr

# Check that parameters from previous population are even and that the
# perturbed parameter is one of the previous parameters incremented
is_valid="false"
while read parameter
do
    # Add previous epsilon and parameter
    sum=$((previous_epsilon + parameter))

    # Take mod 2
    mod=$((sum % 2))

    # If mod is equal to 1, throw error
    if [ "$mod" -eq "1" ]
    then
        echo "Sum of parameter and epsilon is odd" 1>&2
        exit 1
    fi

    if [ "$((parameter+1))" -eq "$perturbed_prmtr" ]
    then
        is_valid="true"
    fi

    # Print 1
    echo 1
done

# If invalid, throw error
if [ ! "$is_valid" == "true" ]
then
    echo "Perturbed parameter is invalid" 1>&2
    exit 1
fi
