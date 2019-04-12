#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s N\n"
                        "Compute sqrt(rand()) N times\n",
                        argv[0]);
        exit(EXIT_FAILURE);
    }

    long long num = strtoll(argv[1], NULL, 0);

    long long i;
    for (i = 0; i < num; i++)
    {
        double val = sqrt(rand());
    }
}
