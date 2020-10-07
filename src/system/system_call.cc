#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <tuple>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "spdlog/spdlog.h"

#include "core/common.h"
#include "core/utils.h"
#include "pipe_io.h"
#include "system_call.h"

const int READ_END = 0;
const int WRITE_END = 1;

std::string get_waitpid_errno()
{
    if (errno == ECHILD)
    {
        return "ECHILD";
    }
    else if (errno == EINVAL)
    {
        return "EINVAL";
    }
    else if (errno == EINTR)
    {
        return "EINTR";
    }
    else
    {
        return "Unrecognised";
    }
}

bool waitpid_success(pid_t pid, int options, const Command& cmd,
                     child_err_opt_t child_err_opt)
{
    // Wait on child
    int status;
    pid_t retval = waitpid(pid, &status, options);

    // Check exit status of retval
    if (retval == 0) // No state change with WNOHANG
        return false;

    if (retval == -1)
    {
        std::string error_msg("waitpid of ");
        error_msg += cmd.str();
        error_msg += " failed";
        error_msg += ", errno = ";
        error_msg += get_waitpid_errno();
        std::runtime_error e(error_msg);
        throw e;
    }

    // Check exit status of child
    if (child_err_opt == throw_error)
    {
        if (!WIFEXITED(status)) // Program did not exit normally
        {
            std::string error_msg(cmd.str());
            error_msg += " did not exit normally";
            std::runtime_error e(error_msg);
            throw e;
        }

        if (WEXITSTATUS(status) != 0) // Check for nonzero exit status
        {
            std::string error_msg(cmd.str());
            error_msg += " threw an error";
            std::runtime_error e(error_msg);
            throw e;
        }
    }

    // Wait was successful
    return true;
}

bool waitpid_success(pid_t pid, int& error_code, int options,
        const Command& cmd)
{
    // Wait on child
    int status;
    pid_t retval = waitpid(pid, &status, options);

    // Check exit status of retval
    if (retval == 0) // No state change with WNOHANG
        return false;

    if (retval == -1)
    {
        std::string error_msg("waitpid of ");
        error_msg += cmd.str();
        error_msg += " failed";
        error_msg += ", errno = ";
        error_msg += get_waitpid_errno();
        std::runtime_error e(error_msg);
        throw e;
    }

    // Check exit status of child
    if (!WIFEXITED(status)) // Program did not exit normally
    {
        std::string error_msg(cmd.str());
        error_msg += " did not exit normally";
        std::runtime_error e(error_msg);
        throw e;
    }

    // Record exit status
    error_code = WEXITSTATUS(status);

    // Wait was successful
    return true;
}

void dup2_check(int oldfd, int newfd)
{

    int retval = dup2(oldfd, newfd);

    if (retval == -1)
    {
        perror("dup2 failed");
        throw;
    }
}

void close_check(int fd)
{

    int retval = close(fd);

    if (retval == -1)
    {
        perror("close failed");
        throw;
    }
}

std::string system_call(const Command& cmd)
{
    // Check if cmd is executable
    if (!cmd.isExecutable())
    {
        std::string error_msg;
        error_msg += "cannot access '";
        error_msg += cmd.argv()[0];
        error_msg += "'";
        perror(error_msg.c_str());
        throw;
    }

    spdlog::debug("cmd: {}", cmd.str());

    // Initialize output
    std::string output;

    // Create pipe
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        std::runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        std::runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) // I am the parent
    {

        // Close write end of pipe
        close_check(pipefd[WRITE_END]);

        // Read from pipe and save in output
        read_from_pipe(pipefd, output);

        // Close read end of pipe
        close_check(pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, 0, cmd);

    }
    else // I am the child
    {

        // Supress stdin of child process
        int devnull = open("/dev/null", O_RDONLY);
        dup2_check(devnull, STDIN_FILENO);
        close_check(devnull);

        // Close read end of pipe and redirect stdout to write end
        close_check(pipefd[READ_END]);
        dup2_check(pipefd[WRITE_END], 1);
        close_check(pipefd[WRITE_END]);

        // Suppress stderr of child process
        if (g_discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Get argv from command
        char **argv = cmd.argv();

        // Execute command
        execvp(argv[0], argv);

        // If program reaches this, command execution has failed
        std::string error_msg("exec of ");
        error_msg += cmd.str();
        error_msg += " failed";
        std::runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);

    return output;
}

std::string system_call(const Command& cmd, const std::string& input)
{
    // Check if cmd is executable
    if (!cmd.isExecutable())
    {
        std::string error_msg;
        error_msg += "cannot access '";
        error_msg += cmd.argv()[0];
        error_msg += "'";
        perror(error_msg.c_str());
        throw;
    }

    spdlog::debug("cmd: {}", cmd.str());
    spdlog::debug("input: {}", input);

    // Initialize output
    std::string output;

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        std::runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        std::runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) // I am the parent
    {

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Send input to child
        write_to_pipe(send_pipefd, input);
        close_check(send_pipefd[WRITE_END]);

        // Read output from child
        read_from_pipe(recv_pipefd, output);
        close_check(recv_pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, 0, cmd);

    }
    else // I am the child
    {

        // Close write end of send pipe and redirect stdin to read end
        // of send pipe
        close_check(send_pipefd[WRITE_END]);
        dup2_check(send_pipefd[READ_END], 0);
        close_check(send_pipefd[READ_END]);

        // Close read end of receive pipe and redirect stdout to write end
        // of receive pipe
        close_check(recv_pipefd[READ_END]);
        dup2_check(recv_pipefd[WRITE_END], 1);
        close_check(recv_pipefd[WRITE_END]);

        // Suppress stderr of child process
        if (g_discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Get argv from command
        char **argv = cmd.argv();

        // Execute command
        execvp(argv[0], argv);

        // If program reaches this, command execution has failed
        std::string error_msg("exec of ");
        error_msg += cmd.str();
        error_msg += " failed";
        std::runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);

    return output;
}

std::pair<std::string, int> system_call_error_code(const Command& cmd,
                 const std::string& input)
{
    // Check if cmd is executable
    if (!cmd.isExecutable())
    {
        std::string error_msg;
        error_msg += "cannot access '";
        error_msg += cmd.argv()[0];
        error_msg += "'";
        perror(error_msg.c_str());
        throw;
    }

    spdlog::debug("cmd: {}", cmd.str());
    spdlog::debug("input: {}", input);

    // Initialize output and error_code
    std::string output;
    int error_code;

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        std::runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        std::runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) // I am the parent
    {

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Send input to child
        write_to_pipe(send_pipefd, input);
        close_check(send_pipefd[WRITE_END]);

        // Read output from child
        read_from_pipe(recv_pipefd, output);
        close_check(recv_pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, error_code, 0, cmd);

    }
    else // I am the child
    {

        // Close write end of send pipe and redirect stdin to read end
        // of send pipe
        close_check(send_pipefd[WRITE_END]);
        dup2_check(send_pipefd[READ_END], 0);
        close_check(send_pipefd[READ_END]);

        // Close read end of receive pipe and redirect stdout to write end
        // of receive pipe
        close_check(recv_pipefd[READ_END]);
        dup2_check(recv_pipefd[WRITE_END], 1);
        close_check(recv_pipefd[WRITE_END]);

        // Suppress stderr of child process
        if (g_discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Get argv from command
        char **argv = cmd.argv();

        // Execute command
        execvp(argv[0], argv);

        // If program reaches this, command execution has failed
        std::string error_msg("exec of ");
        error_msg += cmd.str();
        error_msg += " failed";
        std::runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);

    return std::make_pair(std::move(output), error_code);
}

std::tuple<pid_t, int, int> system_call_non_blocking_read_write(
        const Command& cmd)
{
    // Check if cmd is executable
    if (!cmd.isExecutable())
    {
        std::string error_msg;
        error_msg += "cannot access '";
        error_msg += cmd.argv()[0];
        error_msg += "'";
        perror(error_msg.c_str());
        throw;
    }

    spdlog::debug("cmd: {}", cmd.str());

    // Initialize child_pid, pipe_read_fd, pipe_write_fd
    pid_t child_pid;
    int pipe_read_fd, pipe_write_fd;

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        std::runtime_error e("pipe failed");
        throw e;
    }

    // Fork and record child pid
    child_pid = fork();

    if (child_pid == -1)
    {
        std::runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) // I am the parent
    {

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Save write end of send pipe to pipe_write_fd
        pipe_write_fd = send_pipefd[WRITE_END];

        // Save read end of recv pipe to pipe_read_fd
        pipe_read_fd = recv_pipefd[READ_END];

    }
    else // I am the child
    {

        // Close write end of send pipe and redirect stdin to read end
        // of send pipe
        close_check(send_pipefd[WRITE_END]);
        dup2_check(send_pipefd[READ_END], 0);
        close_check(send_pipefd[READ_END]);

        // Close read end of receive pipe and redirect stdout to write end
        // of receive pipe
        close_check(recv_pipefd[READ_END]);
        dup2_check(recv_pipefd[WRITE_END], 1);
        close_check(recv_pipefd[WRITE_END]);

        // Suppress stderr of child process
        if (g_discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Get argv from command
        char **argv = cmd.argv();

        // Execute command
        execvp(argv[0], argv);

        // If program reaches this, command execution has failed
        std::string error_msg("exec of ");
        error_msg += cmd.str();
        error_msg += " failed";
        std::runtime_error e(error_msg);
        throw e;
    }

    return std::make_tuple(child_pid, pipe_write_fd, pipe_read_fd);
}
