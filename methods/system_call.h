#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include <string>
#include <unistd.h>
#include "types.h"

typedef enum { throw_error, ignore_error } child_err_opt_t;

bool waitpid_success(pid_t pid, int options = 0, const cmd_t& cmd = "cmd",
                     child_err_opt_t child_err_opt = throw_error);

void dup2_check(int oldfd, int newfd);
void close_check(int fd);

void system_call(const cmd_t& cmd, std::string& output,
                 child_err_opt_t child_err_opt = throw_error);
void system_call(const cmd_t& cmd, const std::string& input,
                 std::string& output, child_err_opt_t child_err_opt = throw_error);

void system_call(const cmd_t& cmd, pid_t& child_pid, int& pipe_read_fd);
void system_call(const cmd_t& cmd, pid_t& child_pid, int& pipe_write_fd, int& pipe_read_fd);

#endif // SYSTEM_CALL_H
