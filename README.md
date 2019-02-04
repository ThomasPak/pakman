# Pakman

A modular, efficient and portable tool for running parallel approximate Bayesian computation algorithms.

# Requirements

* C++11 compiler
* MPI-3.1 installation
* [CMake](https://cmake.org/) >= version 3.10.2

Pakman has been tested on [OpenMPI](https://www.open-mpi.org/) and
[MPICH](https://www.mpich.org/).  Known issues with OpenMPI and MPICH are
described below.

# Building

To build Pakman, run:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Testing

To test Pakman, run:

```
$ ctest
```

# Testing on multiple nodes

By default, running `ctest` will only test Pakman on the local node.  In order
to test Pakman on multiple nodes, define the CMake variable
`MPIEXEC_HOSTS_FLAGS` to contain the command-line flags you would pass to
`mpiexec` to specify the hosts.  In addition, define `MPIEXEC_MAX_NUMPROCS`
to specify the total number of MPI processes to run.

For example, if you would normally launch 8 processes on `node0` and `node1` in
the following manner:

```
$ mpiexec --hosts node0,node1 -n 8 ...
```

Then you need to run CMake as:

```
...
$ cmake .. -DMPIEXEC_HOSTS_FLAGS="--hosts node0,node1" -DMPIEXEC_MAX_NUMPROCS=8
...
```

# Known issues

% Describe limitations of OpenMPI and MPICH due to weak support of dynamic
process management.
