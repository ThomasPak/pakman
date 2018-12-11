#include <string>
#include <cstring>

#include <mpi.h>
#include <signal.h>

#include "mpi_utils.h"
#include "mpi_common.h"

void string_to_buffer(char*& buffer, const std::string& str) {

    // Create buffer to hold string data
    buffer = new char[str.size() + 1];

    // copy string
    strcpy(buffer, str.c_str());
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

void broadcast_raw_input(const std::string& raw_input) {

    // Broadcast raw input
    char *buffer;
    string_to_buffer(buffer, raw_input);
    Dynamic_Bcast(MPI::COMM_WORLD, buffer, raw_input.size() + 1, MPI::CHAR, MASTER_RANK);
    delete[] buffer;
}

void receive_raw_input(std::string& raw_input) {

    // Receive raw input
    char *buffer;
    Dynamic_Bcast(MPI::COMM_WORLD, buffer, 0, MPI::CHAR, MASTER_RANK);
    raw_input.assign(buffer);
    delete[] buffer;
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
