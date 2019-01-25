#include <string>
#include <thread>

#include "types.h"

#include "mpi_utils.h"
#include "mpi_common.h"
#include "spawn.h"
#include "PersistentMPIWorkerHandler.h"
#include "AbstractWorkerHandler.h"

#ifndef NDEBUG
#include <iostream>
#endif

// Static child communicator of PersistentMPIWorkerHandler is default
// MPI::Intercomm (MPI::COMM_NULL)
MPI::Intercomm PersistentMPIWorkerHandler::m_child_comm;

PersistentMPIWorkerHandler::PersistentMPIWorkerHandler(const cmd_t& command,
        const std::string& input_string) :
    AbstractWorkerHandler(command, input_string)
{
    // Spawn persistent MPI child process if it has not yet been spawned
    if (m_child_comm == MPI::COMM_NULL)
        m_child_comm = spawn_worker(m_command);

    // Write input string to spawned MPI process
    m_child_comm.Send(input_string.c_str(), input_string.size() + 1, MPI::CHAR,
            WORKER_RANK, MANAGER_MSG_TAG);
}

PersistentMPIWorkerHandler::~PersistentMPIWorkerHandler()
{
    // Just discard results, do not terminate persistent MPI child process
    discardResults();
}

void PersistentMPIWorkerHandler::terminate()
{
    // Discard results from persistent MPI child process
    discardResults();

    // Send termination signal
    int signal = TERMINATE_WORKER_SIGNAL;
    m_child_comm.Send(&signal, 1, MPI::INT, WORKER_RANK, MANAGER_SIGNAL_TAG);

    // Free communicator
    m_child_comm.Disconnect();
}

bool PersistentMPIWorkerHandler::isDone()
{
    // Probe for result if result has not yet been received
    if (    !m_result_received &&
            m_child_comm.Iprobe(WORKER_RANK, WORKER_MSG_TAG) )
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

std::string PersistentMPIWorkerHandler::receiveMessage() const
{
    return receive_string(m_child_comm, WORKER_RANK, WORKER_MSG_TAG);
}

int PersistentMPIWorkerHandler::receiveErrorCode() const
{
    return receive_integer(m_child_comm, WORKER_RANK, WORKER_ERROR_CODE_TAG);
}

void PersistentMPIWorkerHandler::discardResults()
{
    // MPI does not provide process control, so
    // we can only wait for the simulation to finish
    // if it has not finished yet
    if (!m_result_received)
    {
        // Timeout if message is not ready yet
        while (!m_child_comm.Iprobe(WORKER_RANK, WORKER_MSG_TAG))
            std::this_thread::sleep_for(MAIN_TIMEOUT);

        // Receive message
        receiveMessage();

        // Receive error code
        receiveErrorCode();

        // Set flag
        m_result_received = true;
    }
}

void PersistentMPIWorkerHandler::terminatePersistent()
{
    // If this function is called, the persistent Worker must be in an idle
    // state, so it is not necessary to discard results from Worker.

    // If m_child_comm is the null communicator, the persistent Worker has
    // already been terminated, so nothing needs to be done.
    if (m_child_comm == MPI::COMM_NULL)
        return;

    // Else, send termination signal to persistent Worker
    int signal = TERMINATE_WORKER_SIGNAL;
    m_child_comm.Send(&signal, 1, MPI::INT, WORKER_RANK, MANAGER_SIGNAL_TAG);

    // Free communicator
    m_child_comm.Disconnect();
}
