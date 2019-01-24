#!/bin/bash
set -euo pipefail

# Chaste dummy simulator
mpirun -n 4 --oversubscribe \
    pakman rejection chaste-simulator.rej 10 --mpi

# Chaste persistent dummy simulator
mpirun -n 4 --oversubscribe \
    pakman rejection chaste-persistent-simulator.rej 10 --mpi --persistent

# Chaste simulator ABC rejection
mpirun -n 4 --oversubscribe \
    pakman rejection epithelial-cell-growth.rej 10 --mpi

# Chaste simulator ABC SMC
mpirun -n 4 --oversubscribe \
    pakman smc epithelial-cell-growth.smc 10 --mpi

# Chaste persistent simulator ABC rejection
mpirun -n 4 --oversubscribe \
    pakman rejection epithelial-cell-growth-persistent.rej 10 --mpi --persistent

# Chaste persistent simulator ABC SMC
mpirun -n 4 --oversubscribe \
    pakman smc epithelial-cell-growth-persistent.smc 10 --mpi --persistent
