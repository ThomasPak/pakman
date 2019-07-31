#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "pakman_mpi_worker.h"

/* Define my_simulator */
int my_simulator(int argc, char *argv[],
        const char* input_string, char **p_output_string)
{
    /* Default output string and error code correspond to simulator that always
     * accepts and exits without error */
    char *output_string = "accept\n";
    int error_code = 0;

    /* Print help */
    if (argc == 2 &&
            ( strcmp(argv[1], "--help") == 0
              || strcmp(argv[1], "-h") == 0 ) )
    {
        printf("Usage: %s [OUTPUT_STRING] [ERROR_CODE]\n", argv[0]);
        return 0;
    }

    /* Process given output string */
    int malloced = 0;
    if (argc >= 2)
    {
        output_string = argv[1];

        /* If output string does not terminate on newline, add one */
        size_t len = strlen(output_string);
        if (output_string[len - 1] != '\n')
        {
            output_string = (char *) malloc((len + 2) * sizeof(char));
            strcpy(output_string, argv[1]);
            output_string[len] = '\n';
            output_string[len + 1] = '\0';
            malloced = 1;
        }
    }

    /* Process given error code */
    if (argc >= 3)
        error_code = atoi(argv[2]);

    /* Throw error if more than two arguments are given */
    if (argc > 3)
    {
        fprintf(stderr, "Error: too many arguments given. Try %s --help.",
                argv[0]);
        return 2;
    }

    /* Allocate memory for output_string */
    *p_output_string = malloc((strlen(output_string) + 1) * sizeof(char));

    /* Copy string */
    strcpy(*p_output_string, output_string);

    /* Free memory if malloced */
    if (malloced)
        free(output_string);

    /* Return exit code */
    return error_code;
}

int main(int argc, char *argv[])
{
    /* Initialize MPI */
    MPI_Init(NULL, NULL);

    /* Run MPI Worker */
    pakman_run_mpi_worker(argc, argv, &my_simulator);

    /* Finalize MPI */
    MPI_Finalize();

    return 0;
}
