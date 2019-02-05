#include <mpi.h>

#include "simple_simulator.hpp"
#include "../include/PakmanMPIWorker.hpp"

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create MPI Worker
    PakmanMPIWorker worker(&simple_simulator, PakmanMPIWorker::PAKMAN_DEFAULT);

    // Run MPI Worker
    worker.run(argc, argv);

    // Finalize MPI
    MPI_Finalize();
}
