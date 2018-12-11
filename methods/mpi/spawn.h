#ifndef SPAWN_H
#define SPAWN_H

#include <mpi.h>
#include "../types.h"

MPI::Intercomm spawn(const cmd_t& cmd, MPI::Info info = {});
MPI::Intercomm spawn_worker(const cmd_t& cmd);

#endif // SPAWN_H
