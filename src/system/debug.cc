#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>

#include "debug.h"

const int NUM_LEVELS = 20;
const int STDERR_FILENO = 2;

void print_stacktrace()
{

    void *array[NUM_LEVELS];
    size_t size;

    // Get void* for all entries on the stack
    size = backtrace(array, NUM_LEVELS);

    // print out frames to stderr
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}

void handler(int sig)
{

    fprintf(stderr, "Error: signal: %s\n", strsignal(sig));
    fprintf(stderr, "Printing stacktrace...");

    print_stacktrace();

    exit(1);
}

void set_handlers()
{
    signal(SIGABRT, handler);
    signal(SIGSEGV, handler);
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    signal(SIGPIPE, handler);
}
