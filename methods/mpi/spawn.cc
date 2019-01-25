#include <vector>
#include <string>
#include <sys/utsname.h>

#include <mpi.h>

#include "parse_cmd.h"
#include "vector_argv.h"
#include "types.h"
#include "mpi_common.h"
#include "spawn.h"

#ifndef NDEBUG
#include <iostream>
#endif

MPI::Intercomm spawn(const cmd_t& cmd, MPI::Info info) {

    // Create command and break into tokens
    std::vector<std::string> cmd_tokens;
    parse_cmd(cmd, cmd_tokens);

    std::vector<const char*> argv;
    vector_argv(cmd_tokens, argv);

    // Spawn single process and return intercomm
    // The argument list to Spawn is shifted by one
    // compared to the exec argument list
    const int maxprocs = 1;
    const int root = 0;
#ifndef NDEBUG
    std::cerr << "Spawning " << argv[0] << "...\n";
#endif
    MPI::Intercomm spawn_intercomm =
        MPI::COMM_SELF.Spawn(argv[0], argv.data() + 1, maxprocs, info, root);
#ifndef NDEBUG
    std::cerr << "Spawn of " << argv[0] << " complete\n";
#endif
    return spawn_intercomm;
}

MPI::Intercomm spawn_worker(const cmd_t& cmd)
{
    // Create MPI::Info object
    MPI::Info info = MPI::Info::Create();

    // Ensure process is spawned on same node if force_host_spawn is set
    if (force_host_spawn)
    {
        struct utsname buf;
        uname(&buf);
        info.Set("host", buf.nodename);
    }

    // Spawn Worker
    MPI::Intercomm child_comm = spawn(cmd, info);

    // Free MPI::Info object
    info.Free();

    return child_comm;
}
