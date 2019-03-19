#ifndef VECTOR_STRTOK_H
#define VECTOR_STRTOK_H

#include <vector>
#include <string>

void vector_strtok(const std::string& str,
                   std::vector<std::string>& str_vector,
                   const std::string& delimiters = " ");

#endif // VECTOR_STRTOK_H
