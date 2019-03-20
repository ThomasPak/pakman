#include <vector>
#include <string>
#include <sys/utsname.h>

#include <mpi.h>

#include "spdlog/spdlog.h"

#include "core/utils.h"
#include "mpi_common.h"
#include "spawn.h"


MPI_Comm spawn(const cmd_t& cmd, MPI_Info info)
{

    // Create command and break into tokens
    std::vector<std::string> cmd_tokens = parse_command(cmd);

    // Create argv
    char **argv = create_c_argv(cmd_tokens);

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

    // Free argv
    free_c_argv(argv);

    // Return intercommunicator
    return spawn_intercomm;
}

MPI_Comm spawn_worker(const cmd_t& cmd)
{
    // Create MPI_Info object
    MPI_Info info;
    MPI_Info_create(&info);

    // Ensure process is spawned on same node if force_host_spawn is set
    if (force_host_spawn)
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
