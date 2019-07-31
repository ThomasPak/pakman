#ifndef PAKMANMPIWORKER_HPP
#define PAKMANMPIWORKER_HPP

#include <iostream>
#include <string>
#include <functional>
#include <mpi.h>

/** A class for implementing Pakman MPI Workers.
 *
 * Since MPI simulators cannot be forked directly when using MPIMaster, the MPI
 * function `MPI_Comm_spawn` is used instead to spawn MPI Workers.  As a
 * consequence, the communication between Pakman and MPI Workers does not
 * happen through system pipes, but through the MPI intercommunicator
 * obtained with `MPI_Comm_get_parent`.
 *
 * Most importantly, the MPI simulator can no longer be considered a black box
 * at the systems-level.  Instead, the simulator must be implemented as a
 * function or function object in C++ (see pakman_mpi_worker.h for C), and
 * passed to the constructor of this class.  The run() method will then
 * communicate with Pakman and execute the simulator function to perform the
 * received simulation tasks.
 *
 * Note that `MPI_Init()` should be called before calling run().  Also, after
 * run() returns, `MPI_Finalize()` should be called.
 *
 * For more information, see
 * @ref mpi-simulator "Implementing an MPI simulator".
 */

class PakmanMPIWorker
{
    public:

        /** Constructor from simulator function.
         *
         * The simulator function must accept four arguments;
         * - **argc**  number of command-line arguments.
         * - **argv**  array of command-line arguments.
         * - **input_string**  input to simulator.
         * - **output_string**  output from simulator.
         *
         * In addition, the simulator function must return an error code.
         *
         * @param simulator  simulator function
         */
        PakmanMPIWorker(std::function<int(int argc, char** argv, const
                    std::string& input_string, std::string& output_string)>
                simulator);

        /** Default destructor does nothing. */
        ~PakmanMPIWorker() = default;

        /** Run the Pakman MPI Worker with the given simulator function.
         *
         * @param argc  number of command-line arguments.
         * @param argv  array of command-line arguments.
         *
         * @return exit code.
         */
        int run(int argc, char*argv[]);

        /** Exit code indicating Worker ran successfully. */
        static constexpr int PAKMAN_EXIT_SUCCESS = 0;

        /** Exit code indicating Worker encountered an error. */
        static constexpr int PAKMAN_EXIT_FAILURE = 1;

    private:

        // Get parent communicator
        static MPI_Comm getParentComm();

        // Receive message from Pakman Manager
        std::string receiveMessage();

        // Receive signal from Pakman Manager
        int receiveSignal();

        // Send message to Pakman Manager
        void sendMessage(const std::string& message_string);

        // Send error code to Pakman Manager
        void sendErrorCode(int error_code);

        // Parent communicator
        MPI_Comm m_parent_comm = MPI_COMM_NULL;

        // Simulator function
        std::function<int(int argc, char** argv, const std::string&
                input_string, std::string& output_string)> m_simulator;

        // Parent communication constants
        static constexpr int PAKMAN_ROOT                    = 0;
        static constexpr int PAKMAN_MANAGER_MSG_TAG         = 2;
        static constexpr int PAKMAN_MANAGER_SIGNAL_TAG      = 3;
        static constexpr int PAKMAN_WORKER_MSG_TAG          = 5;
        static constexpr int PAKMAN_WORKER_ERROR_CODE_TAG   = 6;

        static constexpr int PAKMAN_TERMINATE_WORKER_SIGNAL = 0;
};

// Constructor
PakmanMPIWorker::PakmanMPIWorker(
    std::function<int(int argc, char** argv, const std::string& input_string,
        std::string& output_string)> simulator)
: m_simulator(simulator)
{
}

int PakmanMPIWorker::run(int argc, char*argv[])
{
    // Get parent communicator
    m_parent_comm = getParentComm();

    // Check if I was spawned
    if (m_parent_comm == MPI_COMM_NULL)
    {
        std::cerr << "Pakman Worker error: "
            "MPI Worker was not spawned, exiting...\n";
        return PAKMAN_EXIT_FAILURE;
    }

    // Start loop
    bool continue_loop = true;
    while (continue_loop)
    {
        // Probe for message
        MPI_Status status;
        MPI_Probe(PAKMAN_ROOT, MPI_ANY_TAG, m_parent_comm, &status);

        // Check tag
        switch (status.MPI_TAG)
        {
            case PAKMAN_MANAGER_MSG_TAG:
                {
                // Receive message
                std::string input_string = receiveMessage();

                // Run simulation
                std::string output_string;
                int error_code = m_simulator(argc, argv, input_string,
                        output_string);

                // Send output
                sendMessage(output_string);

                // Send error code
                sendErrorCode(error_code);

                break;
                }
            case PAKMAN_MANAGER_SIGNAL_TAG:
                {
                // Receive signal
                int signal = receiveSignal();

                // Check signal
                switch (signal)
                {
                    case PAKMAN_TERMINATE_WORKER_SIGNAL:
                        {
                        // Set loop condition to false
                        continue_loop = false;
                        break;
                        }
                    default:
                        {
                        std::cerr << "Pakman Worker error: signal not recognised, "
                                "exiting...\n";
                        return PAKMAN_EXIT_FAILURE;
                        }
                }
                break;
                }
            default:
                {
                    std::cerr << "Pakman Worker error: "
                        "tag not recognised, exiting...\n";
                return PAKMAN_EXIT_FAILURE;
                }
        }
    }

    // Disconnect parent communicator
    MPI_Comm_disconnect(&m_parent_comm);

    // Return successful error code
    return PAKMAN_EXIT_SUCCESS;
}

// Get parent communicator
MPI_Comm PakmanMPIWorker::getParentComm()
{
    MPI_Comm parent_comm;
    MPI_Comm_get_parent(&parent_comm);

    return parent_comm;
}

// Receive message from Pakman Manager
std::string PakmanMPIWorker::receiveMessage()
{
    // Probe to get status
    MPI_Status status;
    MPI_Probe(PAKMAN_ROOT, PAKMAN_MANAGER_MSG_TAG, m_parent_comm, &status);

    // Receive string
    int count = 0;
    MPI_Get_count(&status, MPI_CHAR, &count);
    char *buffer = new char[count];
    MPI_Recv(buffer, count, MPI_CHAR, PAKMAN_ROOT, PAKMAN_MANAGER_MSG_TAG,
            m_parent_comm, MPI_STATUS_IGNORE);

    // Return string
    std::string message(buffer);
    delete[] buffer;
    return message;
}

// Receive signal from Pakman Manager
int PakmanMPIWorker::receiveSignal()
{
    // Receive signal
    int signal;
    MPI_Recv(&signal, 1, MPI_INT, PAKMAN_ROOT, PAKMAN_MANAGER_SIGNAL_TAG,
            m_parent_comm, MPI_STATUS_IGNORE);

    // Return signal
    return signal;
}

// Send message to Pakman Manager
void PakmanMPIWorker::sendMessage(const std::string& message_string)
{
    // Send message
    MPI_Send(message_string.c_str(), message_string.size() + 1, MPI_CHAR,
            PAKMAN_ROOT, PAKMAN_WORKER_MSG_TAG, m_parent_comm);
}

// Send error code to Pakman Manager
void PakmanMPIWorker::sendErrorCode(int error_code)
{
    // Send error code
    MPI_Send(&error_code, 1, MPI_INT, PAKMAN_ROOT,
            PAKMAN_WORKER_ERROR_CODE_TAG, m_parent_comm);
}

#endif // PAKMANMPIWORKER_HPP
