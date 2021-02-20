#!/bin/bash
set -euo pipefail

# Read parameter
read parameter

# If there is anymore input, throw error
if read dummy
then
    echo "$0 only accepts one line of input"
    exit 1
fi

# Check number of arguments
if [ "$#" -lt 1 ]
then
    echo "Usage: $0 OFFSET"
    exit 1
fi

# Set offset
offset="$1"

# Add ofset
sum=$((parameter + offset))

# Take mod 2 of parameter
mod=$((sum % 2))

# If mod is equal to 0, return zero
# Else, return one
if [ $mod -eq "0" ]
then
    echo 0
else
    echo 1
fi
