#!/bin/bash
set -euo pipefail

mpirun -n 4 --oversubscribe \
    pakman rejection chaste-simulator.rej 10 --mpi

mpirun -n 4 --oversubscribe \
    pakman rejection chaste-persistent-simulator.rej 10 --mpi --persistent

#mpirun -n 4 --oversubscribe \
#    pakman rejection epithelial-cell-growth.rej 10 --mpi
#
#mpirun -n 4 --oversubscribe \
#    pakman smc epithelial-cell-growth.smc 10 --mpi
