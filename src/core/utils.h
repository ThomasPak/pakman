#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "types.h"

std::vector<std::string> parse_command(const std::string& raw_command);

void vector_delimited(const std::vector<std::string>& str_vector,
                 std::string& delimited_string, const std::string& delimiter = " ");

std::string vector_printf(const std::string& format,
                          const std::vector<std::string>& args,
                          const std::string& token = "%s");

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters = " ");

#endif // UTILS_H
