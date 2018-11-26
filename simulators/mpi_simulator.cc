#include <iostream>
#include <mpi.h>
#include "../include/ParentCommunicator.hpp"

int main() {

    std::cerr << "Entered mpi_simulator.cc!\n";

    // Initialize MPI
    MPI_Init(nullptr, nullptr);

    // Create parent communicator
    ParentCommunicator parent_comm;

    // Receive input
    std::string input = parent_comm.receiveMessage();

    // Display input
    std::cerr << "Input:\n" << input << std::endl;

    std::string result("1");

    // Display result
    std::cerr << "Sending result:\n" << result << std::endl;

    // Send result
    parent_comm.sendMessage(result);

    // Display result
    std::cerr << "Sent result!\n";

    // Disconnect
    parent_comm.disconnect();

    // Finalize MPI
    MPI_Finalize();
}
