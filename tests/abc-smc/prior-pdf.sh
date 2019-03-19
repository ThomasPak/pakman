#!/bin/bash
set -euo pipefail

# Read parameter
read parameter

# If there is another line, throw error
if read dummy
then
    echo "$0 accepts only one line of input"
    exit 1
fi

# Print 1
echo 1
