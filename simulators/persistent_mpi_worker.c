#include <mpi.h>

#include "simple_simulator.h"
#include "../include/pakman_mpi_worker.h"

int main()
{
    /* Initialize MPI */
    MPI_Init(NULL, NULL);

    /* Run MPI Worker */
    pakman_run_mpi_worker(&simple_simulator, PAKMAN_O_PERSISTENT);

    /* Finalize MPI */
    MPI_Finalize();
}
