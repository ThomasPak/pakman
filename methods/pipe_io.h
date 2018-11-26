#ifndef PIPE_IO_H
#define PIPE_IO_H

#include <string>
#include <atomic>

void read_from_pipe(const int pipe_read_fd, std::string& output);
void read_from_pipe(const int pipefd[], std::string& output);
bool poll_read_from_pipe(const int pipe_read_fd, std::string& output);

void write_to_pipe(const int pipe_write_fd, const std::string& input);
void write_to_pipe(const int pipefd[], const std::string& input);

#endif // PIPE_IO_H
