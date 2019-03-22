#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<std::string> parse_command(const std::string& raw_command);

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters = " ");

#endif // UTILS_H
