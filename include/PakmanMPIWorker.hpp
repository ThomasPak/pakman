#ifndef PAKMANMPIWORKER_HPP
#define PAKMANMPIWORKER_HPP

#include <iostream>
#include <string>
#include <functional>
#include <mpi.h>

class PakmanMPIWorker
{
    public:

        // Constructor
        PakmanMPIWorker(std::function<int(int, char**, const std::string&,
                    std::string&)> simulator, int flags);

        // Destructor
        ~PakmanMPIWorker() = default;

        // Run
        int run(int argc, char*argv[]);

        // Options
        static constexpr int PAKMAN_DEFAULT          = 0b000;
        static constexpr int PAKMAN_O_PERSISTENT     = 0b001;
        static constexpr int PAKMAN_O_INITIALIZE_MPI = 0b010;
        static constexpr int PAKMAN_O_FINALIZE_MPI   = 0b100;

        // Exit status of run()
        static constexpr int PAKMAN_EXIT_SUCCESS = 0;
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
        std::function<int(int, char**, const std::string&, std::string&)>
                m_simulator;

        // Flags
        int m_flags = 0;

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
    std::function<int(int, char**, const std::string&, std::string&)> simulator,
        int flags)
: m_simulator(simulator), m_flags(flags)
{
}

int PakmanMPIWorker::run(int argc, char*argv[])
{
    // Initialize MPI if flag is set
    if (m_flags & PAKMAN_O_INITIALIZE_MPI)
        MPI_Init(nullptr, nullptr);

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
#ifndef NDEBUG
                        std::cerr << "DEBUG: Pakman Worker: received "
                            "PAKMAN_TERMINATE_WORKER_SIGNAL\n";
#endif
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

        // If not persistent Worker, set loop condition to false
        if (! (m_flags & PAKMAN_O_PERSISTENT))
            continue_loop = false;
    }

    // Disconnect parent communicator
    MPI_Comm_disconnect(&m_parent_comm);

    // Finalize MPI if flag is set
    if (m_flags & PAKMAN_O_FINALIZE_MPI)
        MPI_Finalize();

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
