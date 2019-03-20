#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "types.h"

std::vector<std::string> parse_command(const std::string& raw_command);

std::vector<const char*> vector_argv(
        const std::vector<std::string>& cmd_tokens);

void vector_delimited(const std::vector<std::string>& str_vector,
                 std::string& delimited_string, const std::string& delimiter = " ");

std::string vector_printf(const std::string& format,
                          const std::vector<std::string>& args,
                          const std::string& token = "%s");

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters = " ");


char** create_c_argv(const std::vector<std::string>& cmd_tokens);
void free_c_argv(char **argv);

#endif // UTILS_H
