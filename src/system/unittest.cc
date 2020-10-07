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
    ///// Test of get_waitpid_errno() /////

    // Test ECHILD (invalid child pid)
    int status;
    int options = 0;
    pid_t retval = waitpid(1, &status, options);

    assert(retval == -1);
    assert(errno == ECHILD);
    assert(get_waitpid_errno() == "ECHILD");

    // Test EINVAL (invalid options)
    options = ~(WNOHANG | WUNTRACED);
    retval = waitpid(1, &status, options);

    assert(retval == -1);
    assert(errno == EINVAL);
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
        retval = waitpid(child_pid, &status, options);
        assert(retval == -1);
        assert(errno == EINTR);
        assert(get_waitpid_errno() == "EINTR");

        // waitpid should return normally
        retval = waitpid(child_pid, &status, options);
        assert(retval == child_pid);
    }

    std::cout << "All tests passed!\n";

    return 0;
}
