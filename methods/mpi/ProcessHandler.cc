#include <string>
#include <thread>
#include <atomic>
#include <stdexcept>

#include <cassert>

#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>

#include "../types.h"
#include "../system_call.h"
#include "../pipe_io.h"

#include "common.h"
#include "spawn.h"
#include "ProcessHandler.h"

#ifndef NDEBUG
#include <iostream>
#endif

AbstractProcessHandler::AbstractProcessHandler(
        const cmd_t& command,
        const std::string& input_string) :
    m_command(command),
    m_input_string(input_string)
{
}

std::string AbstractProcessHandler::getOutput()
{
    assert(isDone());

    return m_output_buffer;
}

ForkedProcessHandler::ForkedProcessHandler(
        const cmd_t& command,
        const std::string& input_string) :
    AbstractProcessHandler(command, input_string)
{

    // Start process
    system_call(m_command, m_child_pid, m_pipe_write_fd, m_pipe_read_fd);

    // Write input string to stdin of process
#ifndef NDEBUG
    std::cerr << "input_string: " << input_string << std::endl;
#endif
    write_to_pipe(m_pipe_write_fd, input_string);
    close_check(m_pipe_write_fd);
}

ForkedProcessHandler::~ForkedProcessHandler()
{

#ifndef NDEBUG
    std::cerr << "Destroying...\n";
#endif
    // Wait on child process if it has not yet been waited for
    if (m_child_pid) waitpid_success(m_child_pid, 0, m_command);

    // Close pipe if not already closed
    if (!m_read_done) close_check(m_pipe_read_fd);
}

void ForkedProcessHandler::terminate()
{

#ifndef NDEBUG
    std::cerr << "Terminating...\n";
#endif

#ifndef NDEBUG
    std::cerr << "Checking whether child process has finished...\n";
#endif
    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_command) )
    {
        m_child_pid = 0;
        return;
    }

#ifndef NDEBUG
    std::cerr << "Sending sigterm to child process with pid " << m_child_pid <<
        "...\n";
#endif
    // Send SIGTERM to child process
    if ( kill(m_child_pid, SIGTERM) )
    {
        std::runtime_error e("an error occurred while trying to terminate "
                             "child process");
        throw e;
    }

#ifndef NDEBUG
    std::cerr << "Sleeping before taking more drastic measures...\n";
#endif
    // Sleep for KILL_TIMEOUT
    std::this_thread::sleep_for(KILL_TIMEOUT);

#ifndef NDEBUG
    std::cerr << "Checking whether sigterm worked...\n";
#endif
    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_command, ignore_error) )
    {
        m_child_pid = 0;
        return;
    }

#ifndef NDEBUG
    std::cerr << "Sending sigkill to child process...\n";
#endif
    // Send SIGKILL to child process
    if ( kill(m_child_pid, SIGKILL) )
    {
        std::runtime_error e("an error occurred while trying to kill "
                             "child process");
        throw e;
    }

#ifndef NDEBUG
    std::cerr << "Waiting on child...\n";
#endif
     waitpid_success(m_child_pid, 0, m_command, ignore_error);
     m_child_pid = 0;
}

bool ForkedProcessHandler::isDone()
{
    // Poll pipe if m_read_done flag is false. If pipe is finished reading,
    // close pipe and set m_read_done flag to true
    if (    !m_read_done &&
            poll_read_from_pipe(m_pipe_read_fd, m_output_buffer) )
    {
        close_check(m_pipe_read_fd);
        m_read_done = true;
    }

    return m_read_done;
}

MPIProcessHandler::MPIProcessHandler(
        const cmd_t& command,
        const std::string& input_string) :
    AbstractProcessHandler(command, input_string)
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

MPIProcessHandler::~MPIProcessHandler()
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

void MPIProcessHandler::terminate()
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

bool MPIProcessHandler::isDone()
{
    // Probe for result if result has not yet been received
    MPI::Status status;
    if (    m_result_received &&
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
