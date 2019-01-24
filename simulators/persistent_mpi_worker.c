#include <mpi.h>

#include "simple_simulator.h"
#include "../include/pakman_mpi_worker.h"

int main(int argc, char *argv[])
{
    /* Initialize MPI */
    MPI_Init(NULL, NULL);

    /* Run MPI Worker */
    pakman_run_mpi_worker(argc, argv, &simple_simulator, PAKMAN_O_PERSISTENT);

    /* Finalize MPI */
    MPI_Finalize();
}
