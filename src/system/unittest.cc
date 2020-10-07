#include <iostream>

#include <assert.h>

#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/types.h>

#include <signal.h>
#include <unistd.h>

#include "system_call.h"

bool g_discard_child_stderr = false;

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "received SIGINT\n";
    }
}

int main()
{
    ///// Test of waitpid_success() and get_waitpid_errno() /////

    // Test ECHILD (invalid child pid)
    int status, error_code;
    int options = 0;

    try
    {
        waitpid_success(1, options, Command("dummy_cmd"), ignore_error);
    }
    catch (const std::runtime_error& e)
    {
        assert(std::string(e.what()) == "waitpid of dummy_cmd failed, errno = ECHILD");
    }

    assert(get_waitpid_errno() == "ECHILD");

    // Test EINVAL (invalid options)
    options = ~(WNOHANG | WUNTRACED);

    try
    {
        waitpid_success(1, error_code, options, Command("dummy_cmd"));
    }
    catch (const std::runtime_error& e)
    {
        assert(std::string(e.what()) == "waitpid of dummy_cmd failed, errno = EINVAL");
    }

    assert(get_waitpid_errno() == "EINVAL");

    // Test EINTR (interrupted by signal)
    options = 0;

    // Set signal handler
    struct sigaction act;

    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);

    // Store parent pid
    pid_t parent_pid = getpid();

    // Fork child
    pid_t child_pid = fork();
    if (child_pid == -1)
    {
        std::cout << "fork() returned error." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) // I am the child
    {
        // Send signal to parent
        if (kill(parent_pid, SIGINT) == -1)
        {
            std::cout << "kill() returned error." << std::endl;
            exit(EXIT_FAILURE);
        }

        return 0;
    }
    else // I am the parent
    {
        // waitpid should be interrupted by SIGINT
        try
        {
            waitpid_success(child_pid, options, Command("dummy_cmd"), ignore_error);
        }
        catch (const std::runtime_error& e)
        {
            assert(std::string(e.what()) == "waitpid of dummy_cmd failed, errno = EINTR");
        }

        assert(get_waitpid_errno() == "EINTR");

        // waitpid should return normally
        assert(waitpid_success(child_pid, error_code, options, Command("dummy_cmd")));
    }

    std::cout << "All tests passed!\n";

    return 0;
}
