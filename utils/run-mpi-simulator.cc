#include <thread>
#include <iostream>
#include <string>
#include <chrono>

#include <libgen.h>

#include <mpi.h>

#include "master/MPIWorkerHandler.h"

// Program name
const char *program_name;

// Global variables
std::chrono::milliseconds MAIN_TIMEOUT(1);
bool force_host_spawn = false;

// Help functions
void help();

int main(int argc, char *argv[])
{
    // Set program_name
    program_name = basename(argv[0]);

    // Check arguments
    if ((argc == 1)
            || (std::string(argv[1]).compare("-h") == 0)
            || (std::string(argv[1]).compare("--help") == 0))
    {
        help();
        return 2;
    }

    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create raw command string
    std::string raw_command;
    for (int i = 1; i < argc; i++)
    {
        raw_command += '\'';
        raw_command += argv[i];
        raw_command += '\'';
        raw_command += " ";
    }

    // Read epsilon and parameter
    std::string epsilon, parameter;
    std::getline(std::cin, epsilon);
    std::getline(std::cin, parameter);

    // Create input string
    std::string input_string;
    input_string += epsilon;
    input_string += '\n';
    input_string += parameter;
    input_string += '\n';

    // Create MPIWorkerHandler
    MPIWorkerHandler mpi_worker_handler(raw_command, input_string);

    // Loop until MPI Worker is done
    while (!mpi_worker_handler.isDone())
        std::this_thread::sleep_for(MAIN_TIMEOUT);

    // Print output string
    std::cout << mpi_worker_handler.getOutput();

    // Save error code
    int error_code = mpi_worker_handler.getErrorCode();

    // Terminate MPI Worker
    MPIWorkerHandler::terminateStatic();

    // Finalize
    MPI_Finalize();

    // Return error_code
    return error_code;
}

void help()
{
    std::string help_string;
    help_string += "Usage: ";
    help_string += program_name;
    help_string += " <mpi simulator>";
    help_string += " <mpi simulator args>...";
    help_string += "\n";

    help_string +=
"Read epsilon and parameter from stdin, launch mpi simulator,\n"
"and pass epsilon and parameter as input to mpi simulator.\n"
"Once the mpi simulator has finished, print output to stdout.\n";

    std::cout << help_string;
}
