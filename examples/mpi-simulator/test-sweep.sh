#!/bin/bash
set -uxo pipefail

touch sweep.out sweep.err

test_failed()
{
    echo "Test $0 in $(pwd) failed!"
    rm sweep.out sweep.err
    exit 1
}

# Exit without error
mpirun -n 4 pakman sweep mpi-simulator-no-error.sweep \
    --mpi-simulation 1> sweep.out 2> sweep.err

# Check exit status
[ $? -ne 0 ] && test_failed

# Check whether standard output is as expected
diff sweep.out - << EOF || test_failed
p
1
2
3
4
5
EOF

# Exit with error
# This should return with nonzero error code, else the test has failed
mpirun -n 4 pakman sweep mpi-simulator-error.sweep \
    --mpi-simulation 1> sweep.out 2> sweep.err

# Check exit status
[ $? -eq 0 ] && test_failed

# Clean up files
rm sweep.out sweep.err
