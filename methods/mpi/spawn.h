#ifndef SPAWN_H
#define SPAWN_H

#include <mpi.h>
#include "../types.h"

MPI::Intercomm spawn(const cmd_t& cmd, MPI::Info info = {});

#endif // SPAWN_H
