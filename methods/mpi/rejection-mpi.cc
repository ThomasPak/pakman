#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include <mpi.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

#include "types.h"
#include "read_input.h"
#include "mpi_utils.h"
#include "mpi_common.h"
#include "main_utils.h"
#include "MPIMaster.h"
#include "ABCRejectionController.h"
#include "Manager.h"
#include "PersistentMPIWorkerHandler.h"

static const char *program_name;

std::chrono::milliseconds MAIN_TIMEOUT(1);
std::chrono::milliseconds KILL_TIMEOUT(100);

bool mpi_simulator = false;
bool persistent_simulator = false;
bool force_host_spawn = false;
bool tolerate_rejections = true;
bool tolerate_errors = false;
bool program_terminated = false;

void usage(int status)
{
    if (status != EXIT_SUCCESS)
        fprintf(stdout, "Try '%s --help' for more info.\n", program_name);
    else
    {
        printf("Usage: %s [OPTION]... INPUT_FILE N\n", program_name);
        std::cout <<
"Run the ABC-rejection method as specified by INPUT_FILE until\n"
"N parameters are accepted.\n";
        std::cout <<
"\nINPUT_FILE must contain the following lines:\n"
"  EPSILON                      value of epsilon\n"
"  SIMULATOR                    simulator command\n"
"  PARAMETER_NAMES              comma-separated list of parameter names\n"
"  PRIOR_SAMPLER                prior sampler command\n";

        std::cout << "\nOptional arguments:\n";
        std::cout <<
"  -t, --main-timeout=TIME      sleep for TIME ms in event loop (default 1)\n"
"  -k, --kill-timeout=TIME      wait for TIME ms before sending SIGKILL\n"
"                               (default 100)\n"
"  -o, --tolerate-errors        tolerate error result from simulator\n"
"  -p, --persistent             simulator is persistent\n"
"  -m, --mpi-simulation         simulator is spawned using MPI\n"
"  -f, --force-host-spawn       force simulator to spawn on same host\n"
"                               as manager (needs -m option)\n";
    }
    exit(status);
}

static struct option const long_options[] =
{
    {"main-timeout", required_argument, nullptr, 't'},
    {"kill-timeout", required_argument, nullptr, 'k'},
    {"tolerate-errors", no_argument, nullptr, 'o'},
    {"persistent", no_argument, nullptr, 'p'},
    {"mpi-simulation", no_argument, nullptr, 'm'},
    {"force-host-spawn", no_argument, nullptr, 'f'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
};

int main(int argc, char *argv[])
{
    // Initialize the MPI environment
    MPI_Init(nullptr, nullptr);

    // Get rank
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Set program name
    program_name = argv[0];

    // Process optional arguments
    int c;
    while ((c = getopt_long(argc, argv, "hk:t:opmf", long_options, nullptr)) != -1)
    {
        switch (c)
        {
            case 't':
                MAIN_TIMEOUT = std::chrono::milliseconds(std::stoi(optarg));
                break;
            case 'k':
                KILL_TIMEOUT = std::chrono::milliseconds(std::stoi(optarg));
                break;
            case 'o':
                tolerate_errors = true;
                break;
            case 'p':
                persistent_simulator = true;
                break;
            case 'm':
                mpi_simulator = true;
                break;
            case 'f':
                force_host_spawn = true;
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            default:
                usage(EXIT_FAILURE);
        }
    }

    // If force_host_spawn, mpi_simulator must also be set
    if (force_host_spawn && ! mpi_simulator)
    {
        std::cerr << "Option -m must be set if -f is set\n";
        usage(EXIT_FAILURE);
    }

    // Process positional arguments
    int argind = optind;
    if (argind == argc)
    {
        std::cerr << "File INPUT_FILE not given.\n";
        usage(EXIT_FAILURE);
    }

    // Open input file
    std::ifstream input_file(argv[argind]);
    if (!input_file.good())
    {
        std::cerr << "An error occured while opening " << argv[argind] << ".\n";
        usage(EXIT_FAILURE);
    }

    // Next positional argument
    argind++;
    if (argind == argc)
    {
        std::cerr << "Number of accepted parameters not given.\n";
        usage(EXIT_FAILURE);
    }

    // Read number of accepted parameters
    const int num_accept = std::stoi(argv[argind]);
    if (num_accept <= 0)
    {
        std::cerr << "Number of accepted parameters must be positive integer.\n";
        usage(EXIT_FAILURE);
    }

    // Check if there are any positional arguments left
    argind++;
    if (argind < argc)
    {
        std::cerr << "Too many arguments given.\n";
        usage(EXIT_FAILURE);
    }

    // Parse stdin and store in input_obj
    rejection::input_t input_obj;
    rejection::read_input(input_file, input_obj);

    // Set signal handler
    set_signal_handler();

    // Determine Worker type
    worker_t worker_type = determine_worker_type(mpi_simulator,
            persistent_simulator);

    // Create Manager object
    Manager manager_obj(input_obj.simulator, worker_type, &program_terminated);

    if (rank == 0)
    {
        // Create MPI master and ABC rejection controller
        std::shared_ptr<MPIMaster> p_master =
            std::make_shared<MPIMaster>(&program_terminated);

        std::shared_ptr<ABCRejectionController> p_controller =
            std::make_shared<ABCRejectionController>(input_obj, num_accept);

        // Associate with each other
        p_master->assignController(p_controller);
        p_controller->assignMaster(p_master);

        // Master & Manger event loop
        while (p_master->isActive() || manager_obj.isActive())
        {
            if (p_master->isActive())
                p_master->iterate();

            if (manager_obj.isActive())
                manager_obj.iterate();

            std::this_thread::sleep_for(MAIN_TIMEOUT);
        }
    }
    else
    {
        // Manager event loop
        while (manager_obj.isActive())
        {
            manager_obj.iterate();

            std::this_thread::sleep_for(MAIN_TIMEOUT);
        }
    }

    // Terminate any remaining persistent Workers
    PersistentMPIWorkerHandler::terminatePersistent();

    // Finalize MPI
    MPI_Finalize();
}
