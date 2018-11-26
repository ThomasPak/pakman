#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <atomic>

extern std::chrono::milliseconds MAIN_TIMEOUT;
extern std::chrono::milliseconds KILL_TIMEOUT;

extern std::atomic<bool> terminate_program;

extern bool mpi_simulator;
extern bool force_host_spawn;
extern bool tolerate_rejections;
extern bool tolerate_errors;

const int MASTER = 0;

/**** Tags ****/
const int PARAMETER_TAG = 0;
const int SIGNAL_TAG = 1;
const int RESULT_TAG = 2;

/**** Signals ****/
// Terminate simulation and exit manager loop
const int TERMINATE_MANAGER_SIGNAL = 0;
// Synchronize with master, terminate simulation
// and switch to next epsilon
const int NEXT_GENERATION_SIGNAL = 1;

/**** Results ****/
const int REJECT = 0;
const int ACCEPT = 1;
const int ERROR = 2;
const int CANCEL = 3;

/*** Sampled parameter status ***/
typedef enum {
    rejected,
    accepted,
    error,
    cancelled,
    busy
} status_t;

#endif // COMMON_H
