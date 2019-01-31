#include <string>
#include <cstring>

#include <mpi.h>
#include <signal.h>

#include "mpi_utils.h"
#include "mpi_common.h"

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

bool iprobe_wrapper(int source, int tag, MPI_Comm comm)
{
    int flag = 0;
    MPI_Iprobe(source, tag, comm, &flag, MPI_STATUS_IGNORE);
    return static_cast<bool>(flag);
}

void set_terminate_flag(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGTERM:
            program_terminated = true;
            break;
    }
}

void set_signal_handler() {

    struct sigaction act;

    act.sa_handler = set_terminate_flag;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);
}

std::string receive_string(const MPI::Comm& comm, int source, int tag)
{
    // Probe to get status
    MPI::Status status;
    comm.Probe(source, tag, status);

    // Receive string
    int count = status.Get_count(MPI::CHAR);
    char *buffer = new char[count];
    comm.Recv(buffer, count, MPI::CHAR, source, tag);

    // Return string
    std::string message(buffer);
    delete[] buffer;
    return message;
}

int receive_integer(const MPI::Comm& comm, int source, int tag)
{
    // Receive integer
    int integer;
    comm.Recv(&integer, 1, MPI::INT, source, tag);

    // Return integer
    return integer;
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
