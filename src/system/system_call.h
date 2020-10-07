#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include <string>
#include <utility>
#include <tuple>

#include <unistd.h>

#include "core/Command.h"

enum child_err_opt_t { throw_error, ignore_error };

std::string get_waitpid_errno();

bool waitpid_success(pid_t pid, int options = 0, const Command& cmd = "cmd",
                     child_err_opt_t child_err_opt = throw_error);
bool waitpid_success(pid_t pid, int& error_code, int options = 0, const
        Command& cmd = "cmd");

void dup2_check(int oldfd, int newfd);
void close_check(int fd);

std::string system_call(const Command& cmd);
std::string system_call(const Command& cmd, const std::string& input);

std::pair<std::string, int> system_call_error_code(const Command& cmd,
        const std::string& input);

std::tuple<pid_t, int, int> system_call_non_blocking_read_write(
        const Command& cmd);

#endif // SYSTEM_CALL_H
