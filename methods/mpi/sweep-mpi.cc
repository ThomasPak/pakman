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

#include "../types.h"
#include "../read_input.h"
#include "mpi_utils.h"
#include "mpi_common.h"
#include "MPIMaster.h"
#include "../SweepController.h"
#include "Manager.h"

static const char *program_name;

std::chrono::milliseconds MAIN_TIMEOUT(1);
std::chrono::milliseconds KILL_TIMEOUT(100);

bool mpi_simulator = false;
bool force_host_spawn = false;
bool tolerate_rejections = false;
bool tolerate_errors = false;
bool program_terminated = false;

void usage(int status) {
    if (status != EXIT_SUCCESS)
        fprintf(stdout, "Try '%s --help' for more info.\n", program_name);
    else {
        printf("Usage: %s [OPTION]... [INPUT]\n", program_name);
        std::cout <<
"Run the parameter sweep as specified by INPUT (default stdin).\n";
        std::cout <<
"\nINPUT must contain the following lines:\n"
"  SIMULATOR                    simulator command\n"
"  PARAMETER_NAMES              comma-separated list of parameter names\n"
"  GENERATOR                    generator command\n";

        std::cout << "\nOptional arguments:\n";
        std::cout <<
"  -t, --main-timeout=TIME      sleep for TIME ms in event loop (default 1)\n"
"  -k, --kill-timeout=TIME      wait for TIME ms before sending SIGKILL\n"
"                               (default 100)\n"
"  -o, --tolerate-errors        tolerate error result from simulator\n"
"  -m, --mpi-simulation         simulator is spawned using MPI\n"
"  -f, --force-host-spawn       force simulator to spawn on same host\n"
"                               as manager (needs -m option)\n";
    }
    exit(status);
}

static struct option const long_options[] = {
    {"main-timeout", required_argument, nullptr, 't'},
    {"kill-timeout", required_argument, nullptr, 'k'},
    {"tolerate-errors", no_argument, nullptr, 'o'},
    {"mpi-simulation", no_argument, nullptr, 'm'},
    {"force-host-spawn", no_argument, nullptr, 'f'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
};

int main(int argc, char *argv[]) {

    using namespace sweep;

    // Set program name
    program_name = argv[0];

    // Process optional arguments
    int c;
    while ((c = getopt_long(argc, argv, "hk:t:omf", long_options, nullptr)) != -1) {
        switch (c) {
            case 't':
                MAIN_TIMEOUT = std::chrono::milliseconds(std::stoi(optarg));
                break;
            case 'k':
                KILL_TIMEOUT = std::chrono::milliseconds(std::stoi(optarg));
                break;
            case 'o':
                tolerate_errors = true;
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
    if (force_host_spawn && ! mpi_simulator) {
        std::cerr << "Option -m must be set if -f is set\n";
        usage(EXIT_FAILURE);
    }

    // Read optional file instead of stdin
    std::ifstream file;
    if (optind < argc) {
        file.open(argv[optind]);
        std::cin.rdbuf(file.rdbuf());
    }

    // Check if too many arguments were given
    if (optind + 1 < argc) {
        std::cerr << "Too many arguments given.\n";
        usage(EXIT_FAILURE);
    }

    // Initialize the MPI environment
    int required = MPI_THREAD_MULTIPLE;
    int provided = MPI::Init_thread(required);
    if (provided < required)
        std::cerr << "Warning: need MPI_THREAD_MULTIPLE thread support\n";

    // Get rank
    const int rank = MPI::COMM_WORLD.Get_rank();

    // Master: read and broadcast stdin
    // Manager: receive stdin from master
    std::string raw_input;
    if (rank == 0) {
        raw_input = std::string(std::istreambuf_iterator<char>(std::cin), {});
        broadcast_raw_input(raw_input);
    } else
        receive_raw_input(raw_input);

    // Parse stdin and store in input_obj
    input_t input_obj;
    std::stringstream sstrm(raw_input);
    read_input(sstrm, input_obj);

    // Set signal handler
    set_signal_handler();

    // Create Manager object
    Manager manager_obj(input_obj.simulator,
            mpi_simulator ? mpi_process : forked_process, &program_terminated);

    if (rank == 0)
    {
        // Create MPI master and sweep controller
        std::shared_ptr<MPIMaster> p_master =
            std::make_shared<MPIMaster>(&program_terminated);

        std::shared_ptr<SweepController> p_controller =
            std::make_shared<SweepController>(input_obj);

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

    // Finalize
    MPI::Finalize();
}
