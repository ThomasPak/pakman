#!/bin/bash
set -euo pipefail

# Process arguments
if [ $# -ne 1 ]
then
    echo "Usage: $0 OFFSET" 1>&2
    echo "Perturb parameter by incrementing it" 1>&2
    echo "by one or two, with equal probabilities" 1>&2
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

# Read parameter
read parameter

# If there is another line, throw error
if read dummy
then
    echo "$0 accepts only two lines of input"
    exit 1
fi

# Add offset and parameter
sum=$((offset + parameter))

# Take mod 2
mod=$((sum % 2))

# If mod is equal to 1, throw error
if [ "$mod" -eq "1" ]
then
    echo "Sum of parameter and offset is odd" 1>&2
    exit 1
fi

# Increment parameter by one or two with equal probabilities
if [ "$RANDOM" -le $((32767/2)) ]
then
    ((parameter++)) || :
else
    ((parameter += 2)) || :
fi

# Print parameter
echo $parameter
