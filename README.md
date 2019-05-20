# Pakman

[![Build Status](https://travis-ci.com/ThomasPak/pakman.svg?branch=master)](https://travis-ci.com/ThomasPak/pakman)

A modular, efficient and portable tool for running parallel approximate
Bayesian computation algorithms.

# Requirements

* C++11 compiler
* MPI-3.1 installation
* [CMake](https://cmake.org/) >= version 3.10.2
* (Optional) [Python](https://www.python.org/) >= version 3.6.7 with
  [matplotlib](https://matplotlib.org/) and [numpy](https://www.numpy.org/)

Pakman has been tested with [OpenMPI](https://www.open-mpi.org/) and
[MPICH](https://www.mpich.org/).  Python is not necessary to build Pakman, but
it is used in some Pakman examples and to create figures.

# Building

To build Pakman, run:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Testing

To test Pakman, run (in the build folder):

```
$ ctest
```

This will run a series of tests to verify that the Pakman build is working
correctly.

To test how well Pakman scales with the number of parallel processes employed,
run (in the build folder):

```
$ cd scaling
$ ./run-scaling.sh
```

This script will benchmark Pakman with a CPU-intensive simulator for different
numbers of parallel instances of the simulator.  The results are saved in the
comma-separated file `scaling.csv`.  In addition, if Python was detected by
CMake, the speedup and efficiency with respect to the number of processes will
be plotted in `speedup.png` and `efficiency.png`, respectively.

# Examples

Examples of how to use Pakman can be found in the folder `examples` inside the
build folder.  See the [wiki](https://github.com/ThomasPak/pakman/wiki) for
further documentation.

* [Biased coin and ABC rejection](https://github.com/ThomasPak/pakman/wiki/Example:-biased-coin-flip-and-ABC-rejection)

# Testing on multiple nodes (advanced)

By default, running `ctest` will only test Pakman on the local node.  Since
Pakman uses MPI for parallelisation however, it is possible to run Pakman on
multiple nodes.  In order to run the above tests and examples on the nodes that
specific to your setup, you need to pass additional information to CMake.

Firstly, you need to define the CMake variable `MPIEXEC_HOSTS_FLAGS` to contain
the command-line flags you would pass to `mpiexec` to specify the hosts.
Secondly, define `MPIEXEC_MAX_NUMPROCS` to specify the total number of MPI
processes to run.

For example, if you would normally launch 8 MPI processes on `node0` and `node1` in
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

When building Pakman with these flags, CMake will automatically insert the
appropriate flags in the `mpiexec` commands for running tests (including the
scaling benchmark) and examples.  Thus, running the above commands as before
will now test Pakman on multiple nodes.
