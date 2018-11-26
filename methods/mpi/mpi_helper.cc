#include <string>
#include <cstring>

#include "mpi_helper.h"

void string_to_buffer(char*& buffer, const std::string& str) {

    // Create buffer to hold string data
    buffer = new char[str.size() + 1];

    // copy string
    strcpy(buffer, str.c_str());
}
