#!/bin/bash
set -euo pipefail

# Process arguments
usage="Usage: $0 DATAFILE"

if [ $# -ne 1 ]; then
    echo $usage 1>&2
    exit 1
fi

datafile=$1

# Process first line of stdin
read epsilon

# Process parameter
read parameter

# Retrieve data
data=$(cat $datafile)

# Simulate binomial model
sim=$(./binomial-sampler <<< $parameter)

# Compare
diff=$((data - sim))
absdiff=$((diff < 0 ? - diff : diff))

accept=$((absdiff <= epsilon))

printf 'data: %s\nsim: %s\n' $data $sim 1>&2
printf 'diff: %s\n' $diff 1>&2
printf 'absdiff: %s\n' $absdiff 1>&2
printf 'accept: %s\n' $accept 1>&2

# Accept -> exit code 1
# Reject -> exit code 0
printf $accept