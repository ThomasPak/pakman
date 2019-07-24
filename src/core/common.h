#ifndef COMMON_H
#define COMMON_H

/** @file common.h
 *
 * Global variables and enumeration types.
 */

/** Global flag for ignoring errors from simulator. */
extern bool ignore_errors;
/** Global flag for discarding standard error from all child processes. */
extern bool discard_child_stderr;
/** Global flag to indicate that program has been terminated. */
extern bool program_terminated;
/** Global variable containing name of program. */
extern const char *program_name;

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
