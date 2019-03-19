#include <string>
#include <vector>
#include <stdexcept>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "spdlog/spdlog.h"

#include "common.h"
#include "types.h"
#include "parse_cmd.h"
#include "vector_argv.h"
#include "pipe_io.h"
#include "system_call.h"

const int READ_END = 0;
const int WRITE_END = 1;

bool waitpid_success(pid_t pid, int options, const cmd_t& cmd,
                     child_err_opt_t child_err_opt)
{

    using namespace std;

    // Wait on child
    int status;
    pid_t retval = waitpid(pid, &status, options);

    // Check exit status of retval
    if (retval == 0) // No state change with WNOHANG
        return false;

    if (retval == -1)
    {
        string error_msg("waitpid of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }

    // Check exit status of child
    if (child_err_opt == throw_error)
    {
        if (!WIFEXITED(status)) { // Program did not exit normally
            string error_msg(cmd);
            error_msg += " did not exit normally";
            runtime_error e(error_msg);
            throw e;
        }

        if (WEXITSTATUS(status) != 0) { // Check for nonzero exit status
            string error_msg(cmd);
            error_msg += " threw an error";
            runtime_error e(error_msg);
            throw e;
        }
    }

    // Wait was successful
    return true;
}

bool waitpid_success(pid_t pid, int& error_code, int options, const cmd_t& cmd)
{

    using namespace std;

    // Wait on child
    int status;
    pid_t retval = waitpid(pid, &status, options);

    // Check exit status of retval
    if (retval == 0) // No state change with WNOHANG
        return false;

    if (retval == -1)
    {
        string error_msg("waitpid of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }

    // Check exit status of child
    if (!WIFEXITED(status)) { // Program did not exit normally
        string error_msg(cmd);
        error_msg += " did not exit normally";
        runtime_error e(error_msg);
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

void system_call(const cmd_t& cmd, std::string& output,
                 child_err_opt_t child_err_opt)
{

    using namespace std;

    spdlog::debug("cmd: {}", cmd);

    // Create pipe
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) { // I am the parent

        // Close write end of pipe
        close_check(pipefd[WRITE_END]);

        // Read from pipe and save in output
        read_from_pipe(pipefd, output);

        // Close read end of pipe
        close_check(pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, 0, cmd, child_err_opt);

    } else { // I am the child

        // Suppress stderr of child process
        if (discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Supress stdin of child process
        int devnull = open("/dev/null", O_RDONLY);
        dup2_check(devnull, STDIN_FILENO);
        close_check(devnull);

        // Close read end of pipe and redirect stdout to write end
        close_check(pipefd[READ_END]);
        dup2_check(pipefd[WRITE_END], 1);
        close_check(pipefd[WRITE_END]);

        // Create command and break into tokens
        vector<string> cmd_tokens;
        parse_cmd(cmd, cmd_tokens);

        vector<const char *> argv;
        vector_argv(cmd_tokens, argv);

        // Execute command
        execvp(argv[0], (char * const *) argv.data());

        // If program reaches this, command execution has failed
        string error_msg("exec of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);
}

void system_call(const cmd_t& cmd,
                 const std::string& input,
                 std::string& output,
                 child_err_opt_t child_err_opt)
{

    using namespace std;

    spdlog::debug("cmd: {}", cmd);
    spdlog::debug("input: {}", input);

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) { // I am the parent

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Send input to child
        write_to_pipe(send_pipefd, input);
        close_check(send_pipefd[WRITE_END]);

        // Read output from child
        output.clear();
        read_from_pipe(recv_pipefd, output);
        close_check(recv_pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, 0, cmd, child_err_opt);

    } else { // I am the child

        // Suppress stderr of child process
        if (discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

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

        // Break up command into tokens
        vector<string> cmd_tokens;
        vector<const char *> argv;

        parse_cmd(cmd, cmd_tokens);
        vector_argv(cmd_tokens, argv);

        // Execute command
        execvp(argv[0], (char * const *) argv.data());

        // If program reaches this, command execution has failed
        string error_msg("exec of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);
}

void system_call(const cmd_t& cmd,
                 const std::string& input,
                 std::string& output,
                 int& error_code)
{

    using namespace std;

    spdlog::debug("cmd: {}", cmd);
    spdlog::debug("input: {}", input);

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        runtime_error e("pipe failed");
        throw e;
    }

    // Fork
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) { // I am the parent

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Send input to child
        write_to_pipe(send_pipefd, input);
        close_check(send_pipefd[WRITE_END]);

        // Read output from child
        output.clear();
        read_from_pipe(recv_pipefd, output);
        close_check(recv_pipefd[READ_END]);

        // Wait on child
        waitpid_success(child_pid, error_code, 0, cmd);

    } else { // I am the child

        // Suppress stderr of child process
        if (discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

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

        // Break up command into tokens
        vector<string> cmd_tokens;
        vector<const char *> argv;

        parse_cmd(cmd, cmd_tokens);
        vector_argv(cmd_tokens, argv);

        // Execute command
        execvp(argv[0], (char * const *) argv.data());

        // If program reaches this, command execution has failed
        string error_msg("exec of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }

    spdlog::debug("output: {}", output);
}

void system_call(const cmd_t& cmd, pid_t& child_pid, int& pipe_read_fd)
{

    using namespace std;

    spdlog::debug("cmd: {}", cmd);

    // Create pipe
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        runtime_error e("pipe failed");
        throw e;
    }

    // Fork and record child pid
    child_pid = fork();

    if (child_pid == -1)
    {
        runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) { // I am the parent

        // Close write end of pipe
        close_check(pipefd[WRITE_END]);

        // Save read end of pipe to pipe_read_fd
        pipe_read_fd = pipefd[READ_END];

    } else { // I am the child

        // Suppress stderr of child process
        if (discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

        // Suppress stdin of child process
        int devnull = open("/dev/null", O_RDONLY);
        dup2_check(devnull, STDIN_FILENO);
        close_check(devnull);

        // Close read end of pipe and redirect stdout to write end
        close_check(pipefd[READ_END]);
        dup2_check(pipefd[WRITE_END], 1);
        close_check(pipefd[WRITE_END]);

        // Create command and break into tokens
        vector<string> cmd_tokens;
        parse_cmd(cmd, cmd_tokens);

        vector<const char *> argv;
        vector_argv(cmd_tokens, argv);

        // Execute command
        execvp(argv[0], (char * const *) argv.data());

        // If program reaches this, command execution has failed
        string error_msg("exec of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }
}

void system_call(const cmd_t& cmd, pid_t& child_pid,
                int& pipe_write_fd, int& pipe_read_fd)
{

    using namespace std;

    spdlog::debug("cmd: {}", cmd);

    // Create pipes for sending and receiving
    int send_pipefd[2], recv_pipefd[2];

    if ( (pipe(send_pipefd) == -1) || (pipe(recv_pipefd) == -1) )
    {
        runtime_error e("pipe failed");
        throw e;
    }

    // Fork and record child pid
    child_pid = fork();

    if (child_pid == -1)
    {
        runtime_error e("fork failed");
        throw e;
    }

    if (child_pid > 0) { // I am the parent

        // Close read end of send pipe and write end of receive pipe
        close_check(send_pipefd[READ_END]);
        close_check(recv_pipefd[WRITE_END]);

        // Save write end of send pipe to pipe_write_fd
        pipe_write_fd = send_pipefd[WRITE_END];

        // Save read end of recv pipe to pipe_read_fd
        pipe_read_fd = recv_pipefd[READ_END];

    } else { // I am the child

        // Suppress stderr of child process
        if (discard_child_stderr)
        {
            int devnull = open("/dev/null", O_WRONLY);
            dup2_check(devnull, STDERR_FILENO);
            close_check(devnull);
        }

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

        // Break up command into tokens
        vector<string> cmd_tokens;
        vector<const char *> argv;

        parse_cmd(cmd, cmd_tokens);
        vector_argv(cmd_tokens, argv);

        // Execute command
        execvp(argv[0], (char * const *) argv.data());

        // If program reaches this, command execution has failed
        string error_msg("exec of ");
        error_msg += cmd;
        error_msg += " failed";
        runtime_error e(error_msg);
        throw e;
    }
}
