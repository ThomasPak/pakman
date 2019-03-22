#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<std::string> parse_command(const std::string& raw_command);

std::vector<std::string> parse_tokens(const std::string& str,
        const std::string& delimiters = " ");

#endif // UTILS_H
