#!/bin/bash
set -euo pipefail

# Read epsilon
read epsilon

# Read parameter
read parameter

# Add epsilon and parameter
sum=$((epsilon + parameter))

# Take mod 2
mod=$((sum % 2))

# If mod is equal to 0, accept
# Else, reject
if [ $mod -eq "0" ]
then
    echo 1
else
    echo 0
fi
