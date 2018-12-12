#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "../../include/pakman_mpi_worker.h"

/* Default output string and error code correspond to simulator that always
 * accepts and exits without error */
char *output_string = "1\n";
int error_code = 0;

/* Define mpi_simulator */
int mpi_simulator(const char* input_string, char **p_output_string)
{
    /* Allocate memory for output_string */
    *p_output_string = malloc((strlen(output_string) + 1) * sizeof(char));

    /* Copy string */
    strcpy(*p_output_string, output_string);

    /* Return exit code */
    return error_code;
}

int main(int argc, char *argv[])
{
    /* Print help */
    if (argc == 2 &&
            ( strcmp(argv[1], "--help") == 0
              || strcmp(argv[1], "-h") == 0 ) )
    {
        printf("Usage: %s [OUTPUT_STRING] [ERROR_CODE]\n", argv[0]);
        return 0;
    }

    /* Process given output string */
    if (argc >= 2)
        output_string = argv[1];

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

    /* Initialize MPI */
    MPI_Init(NULL, NULL);

    /* Run MPI Worker */
    pakman_run_mpi_worker(&mpi_simulator, PAKMAN_DEFAULT);

    /* Finalize MPI */
    MPI_Finalize();

    return 0;
}
