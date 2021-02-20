#!/bin/bash
set -euo pipefail

# Process arguments
if [ $# -ne 1 ]
then
    echo "Usage: $0 OFFSET" 1>&2
    echo "Checks parameters and prints 1 if OK" 1>&2
    echo "Throws an error if (parameter + OFFSET) is odd" 1>&2
    exit 1
fi

offset="$1"

# Read t
read t

# Sanity check: t cannot be 0
if [ "$t" -eq "0" ]
then
    echo "t should not be 0" 1>&2
    exit 1
fi

# Read perturbed parameter
read perturbed_prmtr

# Check that parameters from previous population are even and that the
# perturbed parameter is one of the previous parameters incremented
is_valid="false"
while read parameter
do
    # Add previous epsilon and parameter
    sum=$((offset + parameter))

    # Take mod 2
    mod=$((sum % 2))

    # If mod is equal to 1, throw error
    if [ "$mod" -eq "1" ]
    then
        echo "Sum of parameter and offset is odd" 1>&2
        exit 1
    fi

    if [ "$((parameter+2))" -eq "$perturbed_prmtr" ]
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
