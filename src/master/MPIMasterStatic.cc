#include <thread>
#include <string>
#include <iostream>
#include <memory>

#include <mpi.h>

#include <getopt.h>

#include "core/common.h"
#include "core/utils.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"
#include "system/signal_handler.h"
#include "mpi/mpi_utils.h"
#include "mpi/mpi_common.h"
#include "main/help.h"
#include "controller/AbstractController.h"

#include "Manager.h"
#include "MPIWorkerHandler.h"

#include "MPIMaster.h"

// Global MPI_Info
MPI_Info g_info;

// Static help function
std::string MPIMaster::help()
{
    return
R"(* Help message for 'mpi' master *

Description:
  The MPI master parallelizes instances of the simulator, also called
  "workers", across all launched MPI processes.  This means that every MPI
  process is responsible for spawning workers.  The correspondence between
  workers and MPI processes is one-to-one; launching N MPI processes results in
  N workers running in parallel.

  If no optional arguments are given, the simulator is, by default, assumed to
  be a standard simulator, which means that it communicates with pakman
  through its stdin and stdout.

  If the optional argument --mpi-simulator is given, the simulator is assumed
  to communicate with pakman through MPI.  The MPI simulator must then be
  written with the header pakman_mpi_worker.h or PakmanMPIWorker.hpp.

  In order to maximize the number of CPU cycles devoted to the workers, the MPI
  master is implemented using an event loop.  The time spent sleeping at each
  iteration of the event loop can be adjusted using the optional argument
  --main-timeout.

  When a worker needs to be shut down, for example when the algorithm has
  finished, pakman first sends SIGTERM to the worker.  If the worker has not
  exited after a fixed amount of time, it is killed by sending the SIGKILL
  signal.  The amount of time between sending SIGTERM and SIGKILL can be
  changed using the optional argument --kill-timeout.  This is only meaningful
  for standard simulators because the MPI standard does not support signals for
  processes that are spawned using MPI functions.

  Some MPI implementations do not automatically spawn dynamic MPI processes on
  the same host as the spawning MPI process.  The flag --force-host-spawn tries
  to enforce spawning dynamic MPI processes on the same host by setting the
  "host" key in MPI_Info to the same host as the spawning MPI process.

MPI master options:
  -m, --mpi-simulator          simulator is spawned using MPI
  -f, --force-host-spawn       force MPI simulator to spawn on same host
                               as manager (requires -m option)
  -p, --mpi-info=KEY_VAL_STR   specify key-value pairs for MPI_Info object
                               to MPI_Comm_spawn as
                               'KEY1=VALUE1; KEY2=VALUE2; ...; KEYN=VALUEN'
                               (requires -m option).  The characters '=' and
                               ';' can be escaped using a backslash.
  -t, --main-timeout=TIME      sleep for TIME ms in event loop (default 1)
  -k, --kill-timeout=TIME      wait for TIME ms before sending SIGKILL
                               (default 100)
)";
}

Manager::worker_t get_worker(bool mpi_simulator)
{
    if (mpi_simulator)
    {
        return Manager::mpi_worker;
    }
    else
        return Manager::forked_worker;
}

// Static addLongOptions function
void MPIMaster::addLongOptions(LongOptions& lopts)
{
    lopts.add({"main-timeout", required_argument, nullptr, 't'});
    lopts.add({"kill-timeout", required_argument, nullptr, 'k'});
    lopts.add({"mpi-simulator", no_argument, nullptr, 'm'});
    lopts.add({"force-host-spawn", no_argument, nullptr, 'f'});
    lopts.add({"mpi-info", required_argument, nullptr, 'p'});
}

// Static main function
void MPIMaster::run(controller_t controller, const Arguments& args)
{
    // Initialize flags for mpi simulator and persistence
    bool mpi_simulator = false;

    // Process optional arguments
    if (args.isOptionalArgumentSet("main-timeout"))
    {
        std::string&& arg = args.optionalArgument("main-timeout");
        g_main_timeout = std::chrono::milliseconds(std::stoi(arg));
    }

    if (args.isOptionalArgumentSet("kill-timeout"))
    {
        std::string&& arg = args.optionalArgument("kill-timeout");
        g_kill_timeout = std::chrono::milliseconds(std::stoi(arg));
    }

    if (args.isOptionalArgumentSet("mpi-simulator"))
    {
        mpi_simulator = true;

        if (args.isOptionalArgumentSet("force-host-spawn"))
            g_force_host_spawn = true;
    }
    else if (args.isOptionalArgumentSet("force-host-spawn"))
    {
        std::cout << "Error: option --mpi-simulator must be set "
            "if --force-host-spawn is set\n";
        ::help(mpi, controller, EXIT_FAILURE);
    }
    else if (args.isOptionalArgumentSet("mpi-info"))
    {
        std::cout << "Error: option --mpi-simulator must be set "
            "if --mpi-info is set\n";
        ::help(mpi, controller, EXIT_FAILURE);
    }

    // Initialize the MPI environment
    MPI_Init(nullptr, nullptr);

    // Create MPI_Info if using MPI simulator
    if (mpi_simulator)
    {
        MPI_Info_create(&g_info);

        if (args.isOptionalArgumentSet("mpi-info"))
        {
            std::string&& arg = args.optionalArgument("mpi-info");
            std::map<std::string, std::string> dict = parse_key_value_pairs(arg);

            for (auto it = dict.begin(); it != dict.end(); ++it)
                MPI_Info_set(g_info, it->first.c_str(), it->second.c_str());
        }
    }

    // Get rank
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Set signal handler
    set_signal_handler();

    // Determine Worker type
    Manager::worker_t worker_type =
        get_worker(mpi_simulator);

    // Create controller
    std::shared_ptr<AbstractController>
        p_controller(AbstractController::makeController(controller, args));

    // Create Manager object
    auto p_manager = std::make_shared<Manager>(p_controller->getSimulator(),
            worker_type, &g_program_terminated);

    if (rank == 0)
    {
        // Create MPI master
        auto p_master = std::make_shared<MPIMaster>(&g_program_terminated);

        // Associate with each other
        p_master->assignController(p_controller);
        p_controller->assignMaster(p_master);

        // Master & Manager event loop
        while (p_master->isActive() || p_manager->isActive())
        {
            if (p_master->isActive())
                p_master->iterate();

            if (p_manager->isActive())
                p_manager->iterate();

            std::this_thread::sleep_for(g_main_timeout);
        }
    }
    else
    {
        // Manager event loop
        while (p_manager->isActive())
        {
            p_manager->iterate();

            std::this_thread::sleep_for(g_main_timeout);
        }
    }

    // Destroy Manager and Controller
    p_manager.reset();
    p_controller.reset();

    // Destroy g_info if it was allocated
    if (mpi_simulator)
        MPI_Info_free(&g_info);

    // Terminate any remaining Workers
    MPIWorkerHandler::terminateStatic();

    // Finalize
    MPI_Finalize();
}

// Static cleanup function
void MPIMaster::cleanup()
{
    // Terminate all managers
    int comm_size = get_mpi_comm_world_size();
    int signal = TERMINATE_MANAGER_SIGNAL;

    for (int manager_rank = 1; manager_rank < comm_size; manager_rank++)
        MPI_Send(&signal, 1, MPI_INT, manager_rank,
                MASTER_SIGNAL_TAG, MPI_COMM_WORLD);

    // Terminate Worker associated with MPI process with rank 0
    MPIWorkerHandler::terminateStatic();

    // Finalize MPI if not yet finalized
    int is_finalized = 0;
    MPI_Finalized(&is_finalized);

    if (!is_finalized)
        MPI_Finalize();
}
