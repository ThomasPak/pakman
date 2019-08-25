#include <signal.h>

#include "core/common.h"
#include "signal_handler.h"

void set_terminate_flag(int signal)
{
    switch (signal)
    {
        case SIGINT:
        case SIGTERM:
            g_program_terminated = true;
            break;
    }
}

void set_signal_handler()
{

    struct sigaction act;

    act.sa_handler = set_terminate_flag;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);
}

