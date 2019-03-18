#!/bin/bash
set -euo pipefail

# Process arguments
if [ $# -ne 1 ]
then
    echo "Usage: $0 NUMBER_FILE" 1>&2
    echo "Increments number in NUMBER_FILE and prints it" 1>&2
    echo "If NUMBER_FILE does not exist, create NUMBER_FILE with number 1 and print 1" 1>&2
    exit 1
fi

number_file="$1"

# If there is any input, throw error
if read dummy
then
    echo "$0 does not accept any input"
    exit 1
fi

# Base case, if number_file does not exist,
# set current_number to 0
if [ ! -f "$number_file" ]
then
    current_number="0"

# Else induction step, read current number
else
    current_number=$(cat $number_file)
fi

# Increment current number
((current_number++)) || :

# Record current_number into number_file
echo $current_number > $number_file

# Print incremented number
echo $current_number
