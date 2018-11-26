#ifndef MPI_HELPER_H
#define MPI_HELPER_H

#include <string>
#include <mpi.h>

void string_to_buffer(char*& buffer, const std::string& str);

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

#endif // MPI_HELPER_H
