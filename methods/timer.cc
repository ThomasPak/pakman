#include <chrono>

#include "timer.h"

using namespace std::chrono;

static steady_clock::time_point t1;
static steady_clock::time_point t2;

void start_timer() {
    t1 = steady_clock::now();
}

void stop_timer() {
    t2 = steady_clock::now();
}

double elapsed_time() {
    auto elapsed_time = duration_cast<duration<double>>(t2 - t1);
    return elapsed_time.count();
}
