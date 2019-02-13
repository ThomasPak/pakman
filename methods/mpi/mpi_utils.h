#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <string>
#include <mpi.h>

int get_mpi_comm_world_size();
int get_mpi_comm_world_rank();

bool iprobe_wrapper(int source, int tag, MPI_Comm comm,
        MPI_Status *status = MPI_STATUS_IGNORE);

std::string receive_string(MPI_Comm comm, int source, int tag);
int receive_integer(MPI_Comm comm, int source, int tag);

#endif // MPI_UTILS_H
