#ifndef PIPE_IO_H
#define PIPE_IO_H

#include <string>
#include <poll.h>

void read_from_pipe(const int pipe_read_fd, std::string& output);
void read_from_pipe(const int pipefd[], std::string& output);
void check_poll(struct pollfd *fds, nfds_t nfds, int timeout);
bool poll_read_from_pipe(const int pipe_read_fd, std::string& output);

void write_to_pipe(const int pipe_write_fd, const std::string& input);
void write_to_pipe(const int pipefd[], const std::string& input);

#endif // PIPE_IO_H
