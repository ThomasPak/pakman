#include <string>
#include <thread>
#include <stdexcept>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "core/common.h"
#include "system/system_call.h"
#include "system/pipe_io.h"
#include "mpi/mpi_common.h"

#include "ForkedWorkerHandler.h"

ForkedWorkerHandler::ForkedWorkerHandler(
        const Command& simulator,
        const std::string& input_string) :
    AbstractWorkerHandler(simulator, input_string)
{

    // Start process
    std::tie(m_child_pid, m_pipe_write_fd, m_pipe_read_fd) =
        system_call_non_blocking_read_write(m_simulator);

    // Write input string to stdin of process
    write_to_pipe(m_pipe_write_fd, input_string);
    close_check(m_pipe_write_fd);
}

ForkedWorkerHandler::~ForkedWorkerHandler()
{
    // Wait on child process if it has not yet been waited for
    if (m_child_pid) terminate();

    // Close pipe if not already closed
    if (!m_read_done) close_check(m_pipe_read_fd);
}

void ForkedWorkerHandler::terminate()
{
    // If already terminated, return immediately
    if (!m_child_pid) return;

    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_simulator) )
    {
        m_child_pid = 0;
        return;
    }

    // Send SIGTERM to child process
    if ( kill(m_child_pid, SIGTERM) )
    {
        std::runtime_error e("an error occurred while trying to terminate "
                             "child process");
        throw e;
    }

    // Sleep for g_kill_timeout
    std::this_thread::sleep_for(g_kill_timeout);

    // If simulation has finished, mark by setting m_child_pid to zero
    if ( waitpid_success(m_child_pid, WNOHANG, m_simulator, ignore_error) )
    {
        m_child_pid = 0;
        return;
    }

    // Send SIGKILL to child process
    if ( kill(m_child_pid, SIGKILL) )
    {
        std::runtime_error e("an error occurred while trying to kill "
                             "child process");
        throw e;
    }

     waitpid_success(m_child_pid, 0, m_simulator, ignore_error);
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

        // Get error code
        waitpid_success(m_child_pid, m_error_code, 0);
        m_child_pid = 0;

        m_read_done = true;
    }

    return m_read_done;
}
