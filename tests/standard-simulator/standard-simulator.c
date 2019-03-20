#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    /* Default output string and error code correspond to simulator that always
     * accepts and exits without error */
    char *output_string = "1\n";
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

    /* Read and discard standard input */
    while (getchar() != EOF);

    /* Print output string to stdout and exit with error code */
    fputs(output_string, stdout);
    return error_code;
}
