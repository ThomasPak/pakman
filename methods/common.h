#ifndef COMMON_H
#define COMMON_H

/**** Results ****/
const int REJECT = 0;
const int ACCEPT = 1;
const int ERROR = 2;
const int CANCEL = 3;

typedef enum
{
    forked_worker,
    persistent_forked_worker,
    mpi_worker,
    persistent_mpi_worker
} worker_t;


extern bool ignore_errors;

extern bool program_terminated;

extern const char *program_name;

// Enumerate type for pakman master
// TODO: remove _master when namespaces have been removed
enum master_t
{
    no_master,
    serial_master,
    mpi_master,
};

// Enumerate type for pakman controller
// TODO: remove _controller when namespaces have been removed
enum controller_t
{
    no_controller,
    sweep_controller,
    rejection_controller,
    smc_controller,
};

#endif // COMMON_H
