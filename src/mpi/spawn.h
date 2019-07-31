#ifndef SPAWN_H
#define SPAWN_H

#include <mpi.h>

class Command;

MPI_Comm spawn(const Command& cmd, MPI_Info info = MPI_INFO_NULL);
MPI_Comm spawn_worker(const Command& cmd);

#endif // SPAWN_H
