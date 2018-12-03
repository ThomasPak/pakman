#include <mpi.h>

#include "mpi_common.h"
#include "master.h"
#include "Manager.h"

std::chrono::milliseconds MAIN_TIMEOUT(1);
std::chrono::milliseconds KILL_TIMEOUT(100);

bool mpi_simulator = false;
bool force_host_spawn = false;
bool tolerate_rejections = true;
bool tolerate_errors = false;
bool program_terminated = false;

int rank, size;

int main() {

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get rank and size
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {

        mcmc::master();

    } else {

        ;

    }

    // Finalize
    MPI_Finalize();

}
