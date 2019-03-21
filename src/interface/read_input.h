#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <string>
#include <vector>

std::vector<std::string> parse_csv_list(const std::string& csv_list);
std::vector<std::string> read_lines(std::istream& istrm, const int num_lines);

#endif // READ_INPUT_H
