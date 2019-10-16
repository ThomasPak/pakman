#include <iostream>
#include <string>
#include <mpi.h>

#include "PakmanMPIWorker.hpp"

/** @file mpi-simulator-cpp.cc
 *
 * This program is a dummy MPI simulator that will always return a
 * user-specified output and error code, regardless of the input to the
 * simulator.  If no arguments are given, the MPI simulator outputs "accept"
 * and returns the error code 0.
 */

/** Run dummy simulation
 *
 * By default, this function assigns the string "accept" to output_string and
 * returns the error code 0 (which means no error has occurred).  The output
 * string and error code can be altered by optional arguments given through
 * `argc` and `argv`.
 *
 * @param argc  number of command-line arguments.
 * @param argv  array containing command-line arguments.
 * @param input_string  input to simulator
 * @param output_string  output from simulator
 *
 * @return error code.
 */
int my_simulator(int argc, char *argv[],
        const std::string& input_string, std::string& output_string)
{
    // Default output string and error code correspond to simulator that always
    // accepts and exits without error
    output_string.assign("accept\n");
    int error_code = 0;

    // Print help
    if (argc == 2 &&
            ( std::string(argv[1]).compare("--help") == 0
              || std::string(argv[1]).compare("-h") == 0 ) )
    {
        std::cout << "Usage: " << argv[0] << " [OUTPUT_STRING] [ERROR_CODE]\n";
        return 2;
    }

    // Process given output string
    if (argc >= 2)
    {
        output_string.assign(argv[1]);

        // If output string does not terminate on newline, add one
        if (output_string.back() != '\n')
            output_string += '\n';
    }

    // Process given error code
    if (argc >= 3)
        error_code = std::stoi(argv[2]);

    // Throw error if more than two arguments are given
    if (argc > 3)
    {
        std::cerr << "Error: too many arguments given. Try "
            << argv[0] << " --help.\n";
        return 2;
    }

    // Return exit code
    return error_code;
}

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create MPI Worker
    PakmanMPIWorker worker(&my_simulator);

    // Run MPI Worker
    worker.run(argc, argv);

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
