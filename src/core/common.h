#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <chrono>

/** @file common.h
 *
 * Global variables and enumeration types.
 */

/** Timeout duration in event loop. */
extern std::chrono::milliseconds g_main_timeout;

/** Timeout duration between sending SIGTERM and SIGKILL signals. */
extern std::chrono::milliseconds g_kill_timeout;

/** Global flag for forcing MPI Worker to spawn on same host as its Manager. */
extern bool g_force_host_spawn;

/** Global flag for ignoring errors from simulator. */
extern bool g_ignore_errors;

/** Global flag for discarding standard error from all child processes. */
extern bool g_discard_child_stderr;

/** Global flag to indicate that program has been terminated. */
extern bool g_program_terminated;

/** Global variable containing name of program. */
extern const char *g_program_name;

/** Global variable containing name of output file if given. */
extern std::string g_output_file;

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
