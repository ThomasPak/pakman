#include <string>
#include <thread>

#include "core/common.h"
#include "mpi/mpi_utils.h"
#include "mpi/mpi_common.h"
#include "mpi/spawn.h"

#include "MPIWorkerHandler.h"

// Initialize static child communicator of MPIWorkerHandler to the
// null communicators (MPI_COMM_NULL)
MPI_Comm MPIWorkerHandler::s_child_comm = MPI_COMM_NULL;

MPIWorkerHandler::MPIWorkerHandler(const Command& simulator,
        const std::string& input_string) :
    AbstractWorkerHandler(simulator, input_string)
{
    // Spawn  MPI child process if it has not yet been spawned
    if (s_child_comm == MPI_COMM_NULL)
        s_child_comm = spawn_worker(m_simulator);

    // Write input string to spawned MPI process
    MPI_Send(input_string.c_str(), input_string.size() + 1, MPI_CHAR,
            WORKER_RANK, MANAGER_MSG_TAG, s_child_comm);
}

MPIWorkerHandler::~MPIWorkerHandler()
{
    // Just discard results, do not terminate MPI child process
    discardResults();
}

bool MPIWorkerHandler::isDone()
{
    // Probe for result if result has not yet been received
    if (    !m_result_received &&
            iprobe_wrapper(WORKER_RANK, WORKER_MSG_TAG, s_child_comm))
    {
        // Receive message
        m_output_buffer.assign(receiveMessage());

        // Receive error code
        m_error_code = receiveErrorCode();

        // Set flag
        m_result_received = true;
    }

    return m_result_received;
}

std::string MPIWorkerHandler::receiveMessage() const
{
    return receive_string(s_child_comm, WORKER_RANK, WORKER_MSG_TAG);
}

int MPIWorkerHandler::receiveErrorCode() const
{
    return receive_integer(s_child_comm, WORKER_RANK, WORKER_ERROR_CODE_TAG);
}

void MPIWorkerHandler::discardResults()
{
    // MPI does not provide process control, so
    // we can only wait for the simulation to finish
    // if it has not finished yet
    if (!m_result_received)
    {
        // Timeout if message is not ready yet
        while (!iprobe_wrapper(WORKER_RANK, WORKER_MSG_TAG, s_child_comm))
            std::this_thread::sleep_for(g_main_timeout);

        // Receive message
        receiveMessage();

        // Receive error code
        receiveErrorCode();

        // Set flag
        m_result_received = true;
    }
}

void MPIWorkerHandler::terminateStatic()
{
    // If this function is called, the Worker must be in an idle state, so it
    // is not necessary to discard results from Worker.

    // If s_child_comm is the null communicator, the Worker has already been
    // terminated, so nothing needs to be done.
    if (s_child_comm == MPI_COMM_NULL)
        return;

    // Else, send termination signal to Worker
    int signal = TERMINATE_WORKER_SIGNAL;
    MPI_Send(&signal, 1, MPI_INT, WORKER_RANK, MANAGER_SIGNAL_TAG,
            s_child_comm);

    // Free communicator
    MPI_Comm_disconnect(&s_child_comm);
}
