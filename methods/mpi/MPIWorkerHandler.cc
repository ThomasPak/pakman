#include <string>
#include <thread>

#include <sys/utsname.h>

#include "../types.h"

#include "mpi_utils.h"
#include "mpi_common.h"
#include "spawn.h"
#include "MPIWorkerHandler.h"
#include "AbstractWorkerHandler.h"

#ifndef NDEBUG
#include <iostream>
#endif

MPIWorkerHandler::MPIWorkerHandler(
        const cmd_t& command,
        const std::string& input_string) :
    AbstractWorkerHandler(command, input_string)
{
#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI process constructing...\n";
#endif

    // Create MPI::Info object
    MPI::Info info = MPI::Info::Create();

    // Ensure process is spawned on same node if force_host_spawn is set
    if (force_host_spawn)
    {
        struct utsname buf;
        uname(&buf);
        info.Set("host", buf.nodename);
    }

    m_child_comm = spawn(m_command, info);

    // Free MPI::Info object
    info.Free();

    // Write input string to spawned MPI process
    m_child_comm.Send(input_string.c_str(), input_string.size() + 1, MPI::CHAR,
            WORKER_RANK, MANAGER_MSG_TAG);
}

MPIWorkerHandler::~MPIWorkerHandler()
{
#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI simulation destroying...\n";
#endif
    terminate();

    // Free communicator
    m_child_comm.Disconnect();
}

void MPIWorkerHandler::terminate()
{
#ifndef NDEBUG
    const int rank = MPI::COMM_WORLD.Get_rank();
    const int size = MPI::COMM_WORLD.Get_size();
    std::cerr << "Manager " << rank << "/" << size << ": MPI simulation terminating...\n";
#endif
    // MPI does not provide process control, so
    // we can only wait for the simulation to finish
    // if it has not finished yet
    if (!m_result_received)
    {
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
    MPI::Status status;
    if (    !m_result_received &&
            m_child_comm.Iprobe(WORKER_RANK, WORKER_MSG_TAG, status) )
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
