#ifndef VECTOR_ARGV_H
#define VECTOR_ARGV_H

#include <string>
#include <vector>

std::vector<const char*> vector_argv(
        const std::vector<std::string>& cmd_tokens);

#endif // VECTOR_ARGV_H
