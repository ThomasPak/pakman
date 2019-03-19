#include <vector>
#include <string>
#include <cstring>

#include "vector_strtok.h"

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters)
{

    using namespace std;

    char *c_str = strdup(str.c_str()), *pch;

    pch = strtok(c_str, delimiters.c_str());

    str_vector.clear();
    while (pch != NULL)
    {
        str_vector.push_back(pch);
        pch = strtok(NULL, delimiters.c_str());
    }

    free(c_str);
}
