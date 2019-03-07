#include <string>
#include <thread>

#include "types.h"

#include "mpi_utils.h"
#include "mpi_common.h"
#include "spawn.h"
#include "MPIWorkerHandler.h"
#include "AbstractWorkerHandler.h"

MPIWorkerHandler::MPIWorkerHandler(
        const cmd_t& command,
        const std::string& input_string) :
    AbstractWorkerHandler(command, input_string)
{
    // Spawn MPI child process
    m_child_comm = spawn_worker(m_command);

    // Write input string to spawned MPI process
    MPI_Send(input_string.c_str(), input_string.size() + 1, MPI_CHAR,
            WORKER_RANK, MANAGER_MSG_TAG, m_child_comm);
}

MPIWorkerHandler::~MPIWorkerHandler()
{
    // Terminate MPI process
    terminate();

    // Free communicator
    MPI_Comm_disconnect(&m_child_comm);
}

void MPIWorkerHandler::terminate()
{
    // MPI does not provide process control, so
    // we can only wait for the simulation to finish
    // if it has not finished yet
    if (!m_result_received)
    {
        // Timeout if message is not ready yet
        while (!iprobe_wrapper(WORKER_RANK, WORKER_MSG_TAG, m_child_comm))
            std::this_thread::sleep_for(MAIN_TIMEOUT);

        // Receive message
        receiveMessage();

        // Receive error code
        receiveErrorCode();

        // Set flag
        m_result_received = true;
    }
}

bool MPIWorkerHandler::isDone()
{
    // Probe for result if result has not yet been received
    if (    !m_result_received &&
            iprobe_wrapper(WORKER_RANK, WORKER_MSG_TAG, m_child_comm))
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
    return receive_string(m_child_comm, WORKER_RANK, WORKER_MSG_TAG);
}

int MPIWorkerHandler::receiveErrorCode() const
{
    return receive_integer(m_child_comm, WORKER_RANK, WORKER_ERROR_CODE_TAG);
}
