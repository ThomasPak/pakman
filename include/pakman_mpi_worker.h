#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#define PAKMAN_EXIT_SUCCESS 0
#define PAKMAN_EXIT_FAILURE 1

#define PAKMAN_DEFAULT          0b000
#define PAKMAN_O_PERSISTENT     0b001
#define PAKMAN_O_INITIALIZE_MPI 0b010
#define PAKMAN_O_FINALIZE_MPI   0b100

#define PAKMAN_ROOT                 0
#define PAKMAN_MANAGER_MSG_TAG      2
#define PAKMAN_MANAGER_SIGNAL_TAG   3
#define PAKMAN_WORKER_MSG_TAG       5
#define PAKMAN_WORKER_ERROR_CODE_TAG   6

#define PAKMAN_TERMINATE_WORKER_SIGNAL  0

MPI_Comm pakman_get_parent_comm();

char* pakman_receive_message(MPI_Comm comm);
int pakman_receive_signal(MPI_Comm comm);

void pakman_send_message(MPI_Comm comm, const char *message);
void pakman_send_error_code(MPI_Comm comm, int error_code);

int pakman_run_mpi_worker(
        int (*simulator)(const char *input_string, char **p_output_string),
        int flags);

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
#ifndef NDEBUG
    /* Check that signal is exactly one integer */
    MPI_Status status;
    MPI_Probe(PAKMAN_ROOT, PAKMAN_MANAGER_SIGNAL_TAG, comm, &status);

    int count;
    MPI_Get_count(&status, MPI_INT, &count);
    assert(count == 1);
#endif

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
    MPI_Send(&error_code, 1, MPI_INT, PAKMAN_ROOT, PAKMAN_WORKER_ERROR_CODE_TAG, comm);
}

int pakman_run_mpi_worker(
        int (*simulator)(const char *input_string, char **p_output_string),
        int flags)
{
    /* Initialize MPI if flag is set */
    if (flags & PAKMAN_O_INITIALIZE_MPI)
        MPI_Init(NULL, NULL);

    /* Get parent communicator */
    MPI_Comm parent_comm = pakman_get_parent_comm();

    /* Check if I was spawned */
    if (parent_comm == MPI_COMM_NULL)
    {
        fputs("Pakman Worker error: MPI Worker was not spawned, exiting...\n", stderr);
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
            case PAKMAN_MANAGER_MSG_TAG: ;
                /* Receive message */
                char* input_string = pakman_receive_message(parent_comm);

                /* Run simulation */
                char *output_string;
                int error_code = (*simulator)(input_string, &output_string);

                /* Send output */
                pakman_send_message(parent_comm, output_string);

                /* Send error code */
                pakman_send_error_code(parent_comm, error_code);

                /* Free input and output strings */
                free(input_string);
                free(output_string);

                break;

            case PAKMAN_MANAGER_SIGNAL_TAG: ;
                /* Receive signal */
                int signal = pakman_receive_signal(parent_comm);

                /* Check signal */
                switch (signal)
                {
                    case PAKMAN_TERMINATE_WORKER_SIGNAL:
                        /* Set loop condition to false */
                        continue_loop = 0;
                        break;

                    default:
                        fputs("Pakman Worker error: signal not recognised, "
                                "exiting...\n", stderr);
                        return PAKMAN_EXIT_FAILURE;
                }
                break;
            default:
                fputs("Pakman Worker error: tag not recognised, exiting...\n",
                        stderr);
                return PAKMAN_EXIT_FAILURE;
        }

        /* If not persistent Worker, set loop condition to false */
        if (! (flags & PAKMAN_O_PERSISTENT))
            continue_loop = 0;
    }

    /* Disconnect parent communicator */
    MPI_Comm_disconnect(&parent_comm);

    /* Finalize MPI if flag is set */
    if (flags & PAKMAN_O_FINALIZE_MPI)
        MPI_Finalize();

    /* Return successful error code */
    return PAKMAN_EXIT_SUCCESS;
}
