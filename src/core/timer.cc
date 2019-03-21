#include <chrono>

#include "timer.h"

static std::chrono::steady_clock::time_point t1;
static std::chrono::steady_clock::time_point t2;

void start_timer()
{
    t1 = std::chrono::steady_clock::now();
}

void stop_timer()
{
    t2 = std::chrono::steady_clock::now();
}

double elapsed_time()
{
    auto elapsed_time =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    return elapsed_time.count();
}
