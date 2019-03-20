#include <thread>
#include <string>
#include <iostream>
#include <memory>

#include <mpi.h>

#include <getopt.h>

#include "common.h"
#include "main/help.h"
#include "system/signal_handler.h"
#include "Manager.h"
#include "PersistentMPIWorkerHandler.h"
#include "core/LongOptions.h"
#include "core/Arguments.h"

#include "MPIMaster.h"

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

  If the optional arguments --mpi-simulator and --persistent are given, the
  simulator is assumed to communicate through MPI and be persistent.  This
  means that the simulator is not shut down when it is finished and has the
  same lifetime as the program.  The flag PAKMAN_O_PERSISTENT must then be used
  when running the pakman MPI worker.

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
  -p, --persistent             MPI simulator is persistent
  -f, --force-host-spawn       force MPI simulator to spawn on same host
                               as manager (requires -m option)
  -t, --main-timeout=TIME      sleep for TIME ms in event loop (default 1)
  -k, --kill-timeout=TIME      wait for TIME ms before sending SIGKILL
                               (default 100)
)";
}

worker_t get_worker(bool mpi_simulator, bool persistent_simulator)
{
    if (mpi_simulator)
    {
        if (persistent_simulator)
            return persistent_mpi_worker;
        else
            return mpi_worker;
    }
    else
    {
        if (persistent_simulator)
            return persistent_forked_worker;
        else
            return forked_worker;
    }
}

// Static addLongOptions function
void MPIMaster::addLongOptions(LongOptions& lopts)
{
    lopts.add({"main-timeout", required_argument, nullptr, 't'});
    lopts.add({"kill-timeout", required_argument, nullptr, 'k'});
    lopts.add({"mpi-simulator", no_argument, nullptr, 'm'});
    lopts.add({"persistent", no_argument, nullptr, 'p'});
    lopts.add({"force-host-spawn", no_argument, nullptr, 'f'});
}

// Static main function
void MPIMaster::run(controller_t controller, const Arguments& args)
{
    // Initialize flags for mpi simulator and persistence
    bool mpi_simulator = false;
    bool persistent_simulator = false;

    // Process optional arguments
    if (args.isOptionalArgumentSet("main-timeout"))
    {
        std::string&& arg = args.optionalArgument("main-timeout");
        MAIN_TIMEOUT = std::chrono::milliseconds(std::stoi(arg));
    }

    if (args.isOptionalArgumentSet("kill-timeout"))
    {
        std::string&& arg = args.optionalArgument("kill-timeout");
        KILL_TIMEOUT = std::chrono::milliseconds(std::stoi(arg));
    }

    if (args.isOptionalArgumentSet("mpi-simulator"))
    {
        mpi_simulator = true;

        if (args.isOptionalArgumentSet("persistent"))
            persistent_simulator = true;

        if (args.isOptionalArgumentSet("force-host-spawn"))
            force_host_spawn = true;
    }
    else if (args.isOptionalArgumentSet("force-host-spawn"))
    {
        std::cout << "Error: option --mpi-simulator must be set "
            "if --force_host_spawn is set\n";
        ::help(mpi_master, controller, EXIT_FAILURE);
    }
    else if (args.isOptionalArgumentSet("persistent"))
    {
        std::cout << "Error: option --mpi-simulator must be set "
            "if --persistent is set\n";
        ::help(mpi_master, controller, EXIT_FAILURE);
    }

    // Initialize the MPI environment
    MPI_Init(nullptr, nullptr);

    // Get rank
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Set signal handler
    set_signal_handler();

    // Determine Worker type
    worker_t worker_type = get_worker(mpi_simulator, persistent_simulator);

    // Create controller
    std::shared_ptr<AbstractController>
        p_controller(AbstractController::makeController(controller, args));

    // Create Manager object
    std::shared_ptr<Manager> p_manager =
        std::make_shared<Manager>(p_controller->getSimulator(), worker_type,
                &program_terminated);

    if (rank == 0)
    {
        // Create MPI master
        std::shared_ptr<MPIMaster> p_master =
            std::make_shared<MPIMaster>(&program_terminated);

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

            std::this_thread::sleep_for(MAIN_TIMEOUT);
        }
    }
    else
    {
        // Manager event loop
        while (p_manager->isActive())
        {
            p_manager->iterate();

            std::this_thread::sleep_for(MAIN_TIMEOUT);
        }
    }

    // Destroy Manager and Controller
    p_manager.reset();
    p_controller.reset();

    // Terminate any remaining persistent Workers
    PersistentMPIWorkerHandler::terminatePersistent();

    // Finalize
    MPI_Finalize();
}
