#ifndef SPAWN_H
#define SPAWN_H

#include <mpi.h>
#include "types.h"

MPI_Comm spawn(const cmd_t& cmd, MPI_Info info = MPI_INFO_NULL);
MPI_Comm spawn_worker(const cmd_t& cmd);

#endif // SPAWN_H
