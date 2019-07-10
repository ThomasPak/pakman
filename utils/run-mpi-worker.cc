#include <thread>
#include <iostream>
#include <string>
#include <chrono>

#include <mpi.h>

#include "core/Command.h"
#include "master/MPIWorkerHandler.h"

// Global variables
std::chrono::milliseconds MAIN_TIMEOUT(1);
bool force_host_spawn = false;

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create raw command string
    std::string raw_command;
    for (int i = 1; i < argc; i++)
    {
        raw_command += argv[i];
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
    mpi_worker_handler.terminate();

    // Finalize
    MPI_Finalize();

    // Return error_code
    return error_code;
}
