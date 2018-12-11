#include <string>
#include <thread>
#include <stdexcept>
#include <signal.h>

#include "../types.h"
#include "../system_call.h"
#include "../pipe_io.h"
#include "mpi_common.h"

#include "ForkedWorkerHandler.h"
#include "AbstractWorkerHandler.h"

#ifndef NDEBUG
#include <iostream>
#endif

ForkedWorkerHandler::ForkedWorkerHandler(
        const cmd_t& command,
        const std::string& input_string) :
    AbstractWorkerHandler(command, input_string)
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

ForkedWorkerHandler::~ForkedWorkerHandler()
{

#ifndef NDEBUG
    std::cerr << "Destroying...\n";
#endif
    // Wait on child process if it has not yet been waited for
    if (m_child_pid) terminate();

    // Close pipe if not already closed
    if (!m_read_done) close_check(m_pipe_read_fd);
}

void ForkedWorkerHandler::terminate()
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

bool ForkedWorkerHandler::isDone()
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
