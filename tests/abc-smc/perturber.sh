#!/bin/bash
set -euo pipefail

# Process arguments
if [ $# -ne 1 ]
then
    echo "Usage: $0 EPSILONS" 1>&2
    echo "Perturb parameter by incrementing it" 1>&2
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

# Read parameter
read parameter

# If there is another line, throw error
if read dummy
then
    echo "$0 accepts only two lines of input"
    exit 1
fi

# Get previous epsilon (cut uses one-based indexing for fields)
previous_epsilon=$(echo $epsilon_list | cut -d, -f$t)

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

# Increment parameter
((parameter++)) || :

# Print parameter
echo $parameter
