#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <string>
#include <mpi.h>

int get_mpi_comm_world_size();
int get_mpi_comm_world_rank();

bool iprobe_wrapper(int source, int tag, MPI_Comm comm);

void string_to_buffer(char*& buffer, const std::string& str);
void set_terminate_flag(int signal);
void set_signal_handler();
void broadcast_raw_input(const std::string& raw_input);
void receive_raw_input(std::string& raw_input);

std::string receive_string(const MPI::Comm& comm, int source, int tag);
int receive_integer(const MPI::Comm& comm, int source, int tag);

std::string receive_string(MPI_Comm comm, int source, int tag);
int receive_integer(MPI_Comm comm, int source, int tag);

/*** Templates ***/
template <class T>
void Dynamic_Bcast(const MPI::Comm& comm, T*& buffer,
                   int count, const MPI::Datatype& datatype, int root) {

    // Root broadcasts count
    comm.Bcast(&count, 1, MPI::INT, root);

    // Allocate buffer if not root
    const int rank = MPI::COMM_WORLD.Get_rank();
    if ( rank != root )
        buffer = (T*) new char[count * datatype.Get_size()];

    // Root broadcasts "count" number of elements of datatype
    comm.Bcast(buffer, count, datatype, root);
}


#endif // MPI_UTILS_H
