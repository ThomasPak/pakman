#ifndef C_ARGV_H
#define C_ARGV_H

#include <string>
#include <vector>

char** create_c_argv(const std::vector<std::string>& cmd_tokens);
void free_c_argv(char **argv);

#endif // C_ARGV_H
