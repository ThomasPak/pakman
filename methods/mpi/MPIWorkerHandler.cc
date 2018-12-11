#include <string>
#include <thread>

#include <sys/utsname.h>

#include "../types.h"

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
            0, 0);
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
        // Source and tag are both zero
        const int source = 0, tag = 0;

        MPI::Status status;
        while (!m_child_comm.Iprobe(0, 0, status))
            // Sleep for MAIN_TIMEOUT
            std::this_thread::sleep_for(MAIN_TIMEOUT);

        const int count = status.Get_count(MPI::CHAR);
        char *buffer = new char[count];

        m_child_comm.Recv(buffer, count, MPI::CHAR, source, tag);

        m_result_received = true;

        delete[] buffer;
    }
}

bool MPIWorkerHandler::isDone()
{
    // Probe for result if result has not yet been received
    MPI::Status status;
    if (    !m_result_received &&
            m_child_comm.Iprobe(0, 0, status) )
    {
        // Source and tag are both zero
        const int source = 0, tag = 0;

        const int count = status.Get_count(MPI::CHAR);
        char *buffer = new char[count];

        m_child_comm.Recv(buffer, count, MPI::CHAR, source, tag);

        m_output_buffer.assign(buffer);
        delete[] buffer;

        m_result_received = true;
    }

    return m_result_received;
}
