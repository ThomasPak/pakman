#ifndef COMMON_H
#define COMMON_H

// Global variables
extern bool ignore_errors;
extern bool discard_child_stderr;

extern bool program_terminated;

extern const char *program_name;

// Enumerate type for pakman master
enum master_t
{
    no_master,
    serial,
    mpi,
};

// Enumerate type for pakman controller
enum controller_t
{
    no_controller,
    sweep,
    rejection,
    smc,
};

#endif // COMMON_H
