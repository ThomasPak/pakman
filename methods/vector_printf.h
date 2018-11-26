#ifndef VECTOR_PRINTF_H
#define VECTOR_PRINTF_H

#include <vector>
#include <string>

std::string vector_printf(const std::string& format,
                          const std::vector<std::string>& args,
                          const std::string& token = "%s");

#endif // VECTOR_PRINTF_H
