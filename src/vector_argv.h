#ifndef VECTOR_ARGV_H
#define VECTOR_ARGV_H

#include <string>
#include <vector>

void vector_argv(const std::vector<std::string>& cmd_tokens,
                 std::vector<const char*>& argv);

#endif // VECTOR_ARGV_H
