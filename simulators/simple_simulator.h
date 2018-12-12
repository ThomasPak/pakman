#include <stdlib.h>
#include <string.h>

int simple_simulator(const char* input_string, char **p_output_string)
{
    /* Always accept */
    char *output_string = "1\n";

    /* Allocate memory for output_string */
    *p_output_string = malloc((strlen(output_string) + 1) * sizeof(char));

    /* Copy string */
    strcpy(*p_output_string, output_string);

    /* No errors occurred */
    return 0;
}
