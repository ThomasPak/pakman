#include <vector>
#include <string>
#include <string.h>

#include "c_argv.h"

char** create_c_argv(const std::vector<std::string>& cmd_tokens)
{
    // Get number of tokens
    int num_tokens = cmd_tokens.size();

    // Allocate pointers to tokens + 1 for nullptr
    char **argv = new char*[num_tokens + 1];

    // Loop, allocate strings and copy
    for (int i = 0; i < num_tokens; i++)
    {
        // Get size of string
        int size = cmd_tokens[i].size();

        // Allocate size of string + 1 for terminating null character
        argv[i] = new char[size + 1];

        // Copy string
        strcpy(argv[i], cmd_tokens[i].c_str());
    }

    // Terminating null pointer
    argv[num_tokens] = nullptr;

    return argv;
}

void free_c_argv(char **argv)
{
    // Loop until nullptr encountered
    int i = 0;
    while (argv[i] != nullptr)
    {
        // Free command token
        delete[] argv[i];
        i++;
    }

    // Free argv
    delete[] argv;
}
