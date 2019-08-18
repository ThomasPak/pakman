#ifndef COMMON_H
#define COMMON_H

#include <ostream>
#include <chrono>

/** @file common.h
 *
 * Global variables and enumeration types.
 */

/** Timeout duration in event loop. */
extern std::chrono::milliseconds MAIN_TIMEOUT;

/** Timeout duration between sending SIGTERM and SIGKILL signals. */
extern std::chrono::milliseconds KILL_TIMEOUT;

/** Global flag for forcing MPI Worker to spawn on same host as its Manager. */
extern bool force_host_spawn;

/** Global flag for ignoring errors from simulator. */
extern bool ignore_errors;

/** Global flag for discarding standard error from all child processes. */
extern bool discard_child_stderr;

/** Global flag to indicate that program has been terminated. */
extern bool program_terminated;

/** Global variable containing name of program. */
extern const char *program_name;

/** Global variable pointing to output stream. */
extern std::ostream *p_output_stream;

/** Enumeration type for master type. */
enum master_t
{
    no_master,
    serial,
    mpi,
};

/** Enumeration type for controller type. */
enum controller_t
{
    no_controller,
    sweep,
    rejection,
    smc,
};

#endif // COMMON_H
