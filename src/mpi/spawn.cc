#include <vector>
#include <string>
#include <sys/utsname.h>

#include <mpi.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "core/utils.h"
#include "core/Command.h"
#include "mpi_common.h"
#include "spawn.h"

// Global MPI_Info
extern MPI_Info g_info;

MPI_Comm spawn(const Command& cmd, MPI_Info info)
{
    // Get argv from command
    char **argv = cmd.argv();

    // Spawn single process and return intercomm
    // The argument list to Spawn is shifted by one
    // compared to the exec argument list
    const int maxprocs = 1;
    const int root = 0;

    spdlog::debug("Spawning {}...", argv[0]);

    MPI_Comm spawn_intercomm;
    MPI_Comm_spawn(argv[0], argv + 1, maxprocs, info, root,
            MPI_COMM_SELF, &spawn_intercomm, MPI_ERRCODES_IGNORE);

    spdlog::debug("Spawn of {} complete", argv[0]);

    // Return intercommunicator
    return spawn_intercomm;
}

MPI_Comm spawn_worker(const Command& cmd)
{
    // Create MPI_Info object
    MPI_Info info;
    MPI_Info_dup(g_info, &info);

    // Ensure process is spawned on same node if g_force_host_spawn is set
    if (g_force_host_spawn)
    {
        struct utsname buf;
        uname(&buf);
        MPI_Info_set(info, "host", buf.nodename);
    }

    // Spawn Worker
    MPI_Comm child_comm = spawn(cmd, info);

    // Free MPI_Info object
    MPI_Info_free(&info);

    return child_comm;
}
