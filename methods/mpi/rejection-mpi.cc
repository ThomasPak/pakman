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
#include "common.h"
#include "master.h"
#include "Manager.h"

static const char *program_name;

std::chrono::milliseconds MAIN_TIMEOUT(1);
std::chrono::milliseconds KILL_TIMEOUT(100);

bool mpi_simulator = false;
bool force_host_spawn = false;
bool tolerate_rejections = true;
bool tolerate_errors = false;

void usage(int status) {
    if (status != EXIT_SUCCESS)
        fprintf(stdout, "Try '%s --help' for more info.\n", program_name);
    else {
        printf("Usage: %s [OPTION]... N [INPUT]\n", program_name);
        std::cout <<
"Run the ABC-rejection method as specified by INPUT (default stdin) until\n"
"N parameters are accepted.\n";
        std::cout <<
"\nINPUT must contain the following lines:\n"
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

    using namespace rejection;

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

    // Process positional arguments
    if (optind >= argc) {
        std::cerr << "Number of accepted parameters not given.\n";
        usage(EXIT_FAILURE);
    }

    // Read number of accepted parameters
    const int num_accept = std::stoi(argv[optind]);
    if (num_accept <= 0) {
        std::cerr << "Number of accepted parameters must be nonnegative integer.\n";
        usage(EXIT_FAILURE);
    }

    // Read optional file instead of stdin
    std::ifstream file;
    if (optind + 1 < argc) {
        file.open(argv[optind + 1]);
        std::cin.rdbuf(file.rdbuf());
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

    // Start master in separate thread
    std::thread master_thread;
    if (rank == 0)
        master_thread = std::thread(master, num_accept, input_obj);

    // Start manager routine
    manager(std::vector<std::string>(1, input_obj.epsilon), input_obj.simulator);

    // Join master thread
    if (rank == 0)
        master_thread.join();

    // Finalize
    MPI::Finalize();
}
