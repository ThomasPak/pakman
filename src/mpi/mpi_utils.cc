#include <string>

#include <string.h>

#include <mpi.h>

#include "mpi_common.h"

#include "mpi_utils.h"

int get_mpi_comm_world_size()
{
    int size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
}

int get_mpi_comm_world_rank()
{
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
}

bool iprobe_wrapper(int source, int tag, MPI_Comm comm, MPI_Status *p_status)
{
    int flag = 0;
    MPI_Iprobe(source, tag, comm, &flag, p_status);
    return static_cast<bool>(flag);
}

std::string receive_string(MPI_Comm comm, int source, int tag)
{
    // Probe to get status
    MPI_Status status;
    MPI_Probe(source, tag, comm, &status);

    // Receive string
    int count = 0;
    MPI_Get_count(&status, MPI_CHAR, &count);
    char *buffer = new char[count];
    MPI_Recv(buffer, count, MPI_CHAR, source, tag, comm, MPI_STATUS_IGNORE);

    // Return string
    std::string message(buffer);
    delete[] buffer;
    return message;
}

int receive_integer(MPI_Comm comm, int source, int tag)
{
    // Receive integer
    int integer;
    MPI_Recv(&integer, 1, MPI_INT, source, tag, comm, MPI_STATUS_IGNORE);

    // Return integer
    return integer;
}
