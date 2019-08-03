#ifndef PAKMAN_MPI_WORKER_H
#define PAKMAN_MPI_WORKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

/** @file pakman_mpi_worker.h
 *
 * Since MPI simulators cannot be forked directly when using MPIMaster, the MPI
 * function `MPI_Comm_spawn` is used instead to spawn MPI Workers.  As a
 * consequence, the communication between Pakman and MPI Workers does not
 * happen through system pipes, but through the MPI intercommunicator
 * obtained with `MPI_Comm_get_parent`.
 *
 * Most importantly, the MPI simulator can no longer be considered a black box
 * at the systems-level.  Instead, the simulator must be implemented as a
 * function in C (see PakmanMPIWorker.hpp for C++).  Its function pointer is
 * then passed to the pakman_run_mpi_worker(), which will then communicate with
 * Pakman and execute the given simulator function to perform the received
 * simulation tasks.
 *
 * Note that `MPI_Init()` should be called before calling
 * pakman_run_mpi_worker().  Also, after pakman_run_mpi_worker() returns,
 * `MPI_Finalize()` should be called.
 *
 * For more information, see
 * @ref mpi-simulator "Implementing an MPI simulator".
 */

/** Exit code indicating Worker ran successfully. */
#define PAKMAN_EXIT_SUCCESS 0

/** Exit code indicating Worker encountered an error. */
#define PAKMAN_EXIT_FAILURE 1

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define PAKMAN_ROOT                     0
#define PAKMAN_MANAGER_MSG_TAG          2
#define PAKMAN_MANAGER_SIGNAL_TAG       3
#define PAKMAN_WORKER_MSG_TAG           5
#define PAKMAN_WORKER_ERROR_CODE_TAG    6

#define PAKMAN_TERMINATE_WORKER_SIGNAL  0

MPI_Comm pakman_get_parent_comm();

char* pakman_receive_message(MPI_Comm comm);
int pakman_receive_signal(MPI_Comm comm);

void pakman_send_message(MPI_Comm comm, const char *message);
void pakman_send_error_code(MPI_Comm comm, int error_code);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/** Run the Pakman MPI Worker with the given simulator function.
 *
 * The simulator function must accept four arguments;
 * - **argc**  number of command-line arguments.
 * - **argv**  array of command-line arguments.
 * - **input_string**  input to simulator.
 * - **p_output_string**  pointer to output from simulator.
 *
 * The returned *p_output_string must have been allocated using `malloc()`.
 * After sending the contents of *p_output_string to Pakman, the MPI Worker
 * will call `free()` on *p_output_string.
 *
 * In addition, the simulator function must return an error code.
 *
 * @param argc  number of command-line arguments.
 * @param argv  array of command-line arguments.
 * @param simulator  function pointer to simulator function.
 *
 * @return exit code.
 */
int pakman_run_mpi_worker(
        int argc, char *argv[],
        int (*simulator)(int argc, char *argv[],
            const char *input_string, char **p_output_string));

#ifndef DOXYGEN_SHOULD_SKIP_THIS

MPI_Comm pakman_get_parent_comm()
{
    MPI_Comm parent_comm;
    MPI_Comm_get_parent(&parent_comm);

    return parent_comm;
}

char* pakman_receive_message(MPI_Comm comm)
{
    /* Probe for message */
    MPI_Status status;
    MPI_Probe(PAKMAN_ROOT, PAKMAN_MANAGER_MSG_TAG, comm, &status);

    /* Allocate buffer to receive message */
    int count;
    MPI_Get_count(&status, MPI_CHAR, &count);
    char *buffer = (char *) malloc(count * sizeof(char));

    /* Receive message */
    MPI_Recv(buffer, count, MPI_CHAR, PAKMAN_ROOT, PAKMAN_MANAGER_MSG_TAG,
            comm, MPI_STATUS_IGNORE);

    return buffer;
}

int pakman_receive_signal(MPI_Comm comm)
{
    /* Allocate buffer to receive message */
    int signal;

    /* Receive message */
    MPI_Recv(&signal, 1, MPI_INT, PAKMAN_ROOT, PAKMAN_MANAGER_SIGNAL_TAG,
            comm, MPI_STATUS_IGNORE);

    return signal;
}

void pakman_send_message(MPI_Comm comm, const char *message)
{
    /* Count length of output_string, including null-terminating character */
    size_t count = strlen(message) + 1;

    /* Send message */
    MPI_Send(message, count, MPI_CHAR, PAKMAN_ROOT, PAKMAN_WORKER_MSG_TAG,
            comm);
}

void pakman_send_error_code(MPI_Comm comm, int error_code)
{
    /* Send error_code */
    MPI_Send(&error_code, 1, MPI_INT, PAKMAN_ROOT,
            PAKMAN_WORKER_ERROR_CODE_TAG, comm);
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

int pakman_run_mpi_worker(
        int argc, char *argv[],
        int (*simulator)(int argc, char *argv[],
            const char *input_string, char **p_output_string))
{
    /* Get parent communicator */
    MPI_Comm parent_comm = pakman_get_parent_comm();

    /* Check if I was spawned */
    if (parent_comm == MPI_COMM_NULL)
    {
        fputs("Pakman Worker error: MPI Worker was not spawned, exiting...\n",
                stderr);
        return PAKMAN_EXIT_FAILURE;
    }

    /* Start loop */
    int continue_loop = 1;
    while (continue_loop)
    {
        /* Probe for message */
        MPI_Status status;
        MPI_Probe(PAKMAN_ROOT, MPI_ANY_TAG, parent_comm, &status);

        /* Check tag */
        switch (status.MPI_TAG)
        {
            case PAKMAN_MANAGER_MSG_TAG:
                {
                /* Receive message */
                char* input_string = pakman_receive_message(parent_comm);

                /* Run simulation */
                char *output_string;
                int error_code = (*simulator)(argc, argv,
                        input_string, &output_string);

                /* Send output */
                pakman_send_message(parent_comm, output_string);

                /* Send error code */
                pakman_send_error_code(parent_comm, error_code);

                /* Free input and output strings */
                free(input_string);
                free(output_string);

                break;
                }
            case PAKMAN_MANAGER_SIGNAL_TAG:
                {
                /* Receive signal */
                int signal = pakman_receive_signal(parent_comm);

                /* Check signal */
                switch (signal)
                {
                    case PAKMAN_TERMINATE_WORKER_SIGNAL:
                        {
                        /* Set loop condition to false */
                        continue_loop = 0;
                        break;
                        }
                    default:
                        {
                        fputs("Pakman Worker error: signal not recognised, "
                                "exiting...\n", stderr);
                        return PAKMAN_EXIT_FAILURE;
                        }
                }
                break;
                }
            default:
                {
                fputs("Pakman Worker error: tag not recognised, exiting...\n",
                        stderr);
                return PAKMAN_EXIT_FAILURE;
                }
        }
    }

    /* Disconnect parent communicator */
    MPI_Comm_disconnect(&parent_comm);

    /* Return successful error code */
    return PAKMAN_EXIT_SUCCESS;
}

#endif /* PAKMAN_MPI_WORKER_H */
