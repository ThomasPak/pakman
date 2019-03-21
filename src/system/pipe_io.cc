#include <string>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include "pipe_io.h"

const int READ_END = 0;
const int WRITE_END = 1;

const int BUFFER_SIZE = 256;

void read_from_pipe(const int pipefd[], std::string& output)
{
    read_from_pipe(pipefd[READ_END], output);
}

void read_from_pipe(const int pipe_read_fd, std::string& output)
{
    ssize_t count;
    char buffer[BUFFER_SIZE];

    output.clear();
    while ( (count = read(pipe_read_fd, buffer, BUFFER_SIZE)) > 0 )
        output.append(buffer, count);

    if (count == -1)
    {
        perror("read failed");
        throw;
    }

}

void check_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{

    int retval = poll(fds, nfds, timeout);

    if (retval == -1)
    {

        // Allow interrupts
        if (errno == EINTR) return;

        perror("close failed");
        throw;
    }
}

/*
 * If read from pipe is finished, return true, else false
 */
bool poll_read_from_pipe(const int pipe_read_fd, std::string& output)
{
    // Polling struct
    struct pollfd fds;
    fds.fd = pipe_read_fd;
    fds.events = POLLIN;

    // Initialize buffers and output
    ssize_t count;
    char buffer[BUFFER_SIZE];

    // Poll
    check_poll(&fds, 1, 0);

    // Check if data is available or pipe was closed
    if ((fds.revents & POLLIN) || (fds.revents & POLLHUP))
    {

        // Even if pipe was closed, there may still be data to read
        while ((count = read(pipe_read_fd, buffer, BUFFER_SIZE)) > 0)
        {

            if (count == -1)
            {
                perror("read failed");
                throw;
            }

            output.append(buffer, count);
        }

        // If pipe was closed, return true
        if (fds.revents & POLLHUP) return true;
    }

    // Pipe was not closed so return false
    return false;
}

void write_to_pipe(const int pipefd[], const std::string& input)
{
    write_to_pipe(pipefd[WRITE_END], input);
}

void write_to_pipe(const int pipe_write_fd, const std::string& input)
{
    ssize_t write_status = write(pipe_write_fd, input.data(), input.size());

    if (write_status == -1)
    {
        std::runtime_error e("write to pipe failed");
        throw e;
    } else if ( write_status < input.size() )
    {
        std::runtime_error e("failed to finish write to pipe");
        throw e;
    }
}
