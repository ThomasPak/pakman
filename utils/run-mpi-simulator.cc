#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

#include <libgen.h>

#include <mpi.h>

#include "master/MPIWorkerHandler.h"

// Program name
const char *g_program_name;

// Global variables
std::chrono::milliseconds g_main_timeout(1);
bool g_force_host_spawn = false;

// Help functions
void help();

int main(int argc, char *argv[])
{
    // Set program_name
    g_program_name = basename(argv[0]);

    // Check arguments
    if ((argc < 3)
            || (std::string(argv[1]).compare("-h") == 0)
            || (std::string(argv[1]).compare("--help") == 0))
    {
        help();

        if ((argc == 2) &&
               ((std::string(argv[1]).compare("-h") == 0)
            || (std::string(argv[1]).compare("--help") == 0)))
            return 0;
        else
            return 2;
    }

    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create raw command string
    std::string raw_command;
    for (int i = 2; i < argc; i++)
    {
        raw_command += '\'';
        raw_command += argv[i];
        raw_command += '\'';
        raw_command += " ";
    }

    // Open input file
    std::ifstream input_file(argv[1]);

    // Read epsilon and parameter
    std::string epsilon, parameter;
    std::getline(input_file, epsilon);
    std::getline(input_file, parameter);

    // Check and close input file
    if (input_file.fail())
    {
        std::string error_msg;
        error_msg += "Error occurred when attempting to read ";
        error_msg += argv[1];
        error_msg += ". ";
        error_msg += "Make sure that ";
        error_msg += argv[1];
        error_msg += " is readable and contains an epsilon value on the first line";
        error_msg += " and a parameter value on the second line.";
        throw std::runtime_error(error_msg);
    }
    input_file.close();

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
        std::this_thread::sleep_for(g_main_timeout);

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
    help_string += g_program_name;
    help_string += " <input file>";
    help_string += " <mpi simulator>";
    help_string += " <mpi simulator args>...";
    help_string += "\n";

    help_string +=
R"(
Read epsilon and parameter from input file, launch mpi simulator,
and pass epsilon and parameter as input to mpi simulator.

The epsilon value should be on the first line of the input file.
The parameter value should be on the second line of the input file.

Once the mpi simulator has finished, print output to stdout.
)";

    std::cout << help_string;
}
